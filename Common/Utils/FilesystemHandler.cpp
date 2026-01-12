#include "FilesystemHandler.h"
#include "string_utils.h"

#include "EASTL/algorithm.h"
#include "EASTL/sort.h"

namespace SEGS
{
RootFilesystem::~RootFilesystem() {}
// FilesystemRepository implementation
eastl::shared_ptr<IFilesystem> FilesystemRepository::getOrCreate(const String &path, const String &extension)
{
    std::lock_guard<std::mutex> lock(mutex_);
    // Check if we have a weak reference that's still valid
    auto it = filesystems_.find(path);
    if (it != filesystems_.end())
    {
        auto fs = it->second.lock();
        if (fs)
        {
            // Found existing instance
            return fs;
        }
        // Weak reference expired, will create a new one
        filesystems_.erase(it);
    }

    // Create a new filesystem and store a weak reference
    auto fs = createFilesystem(path, extension);
    if (fs)
    {
        filesystems_[path] = fs;
    }
    return fs;
}

void FilesystemRepository::registerFactory(const String &extension, FilesystemFactory factory)
{
    std::lock_guard<std::mutex> lock(mutex_);
    factories_[extension] = eastl::move(factory);
}

size_t FilesystemRepository::getActiveInstanceCount() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    size_t count = 0;
    for (const auto &[key, weakFs] : filesystems_)
    {
        if (!weakFs.expired())
        {
            count++;
        }
    }
    return count;
}

eastl::shared_ptr<IFilesystem> FilesystemRepository::createFilesystem(const String &path, const String &extension)
{
    // Find a factory for this extension
    auto it = factories_.find(extension);
    if (it != factories_.end())
    {
        return it->second(path);
    }

    // no factory for filesystem ? won't allow access
    return nullptr;
}

String FilesystemRepository::getFileExtension(const String &path)
{
    size_t pos = path.find_last_of('.');
    if (pos == String::npos)
        return "";
    return path.substr(pos + 1);
}

// RootFilesystem implementation
RootFilesystem::RootFilesystem() : repository_(eastl::make_shared<FilesystemRepository>()) {}

void RootFilesystem::registerFactory(const String &extension, FilesystemFactory factory)
{
    repository_->registerFactory(extension, eastl::move(factory));
}

bool RootFilesystem::mount(const String &sourcePath, const String &mountPoint, int priority)
{
    std::lock_guard lock(mountMutex_);

    // Reject single-letter mount points (A: through Z:) - reserved for Windows drive letters
    if (mountPoint.length() == 2 &&
        CharUtils::is_ascii_char(mountPoint[0]) &&
        mountPoint[1] == ':')
    {
        return false;
    }

    auto normalizedMount = normalizePath(mountPoint);

    // Normalize source path (handles Windows drive letters: C:/path -> /C/path)
    String normalizedSource = PathUtils::internalizePath(sourcePath);

    // Get or create the mount vector for this path
    auto &mounts = mountPoints_[normalizedMount];

    // Check if this source is already mounted here
    auto existingMount =
        eastl::find_if(mounts.begin(), mounts.end(), [&](const MountPoint &mp) { return mp.sourcePath == normalizedSource; });

    if (existingMount != mounts.end())
    {
        // Update priority of existing mount if needed
        if (existingMount->priority != priority)
        {
            existingMount->priority = priority;
            eastl::sort(mounts.begin(), mounts.end());
        }
        return true;
    }

    // Extract extension
    size_t dotPos    = normalizedSource.find_last_of('.');
    String extension = (dotPos != String::npos) ? normalizedSource.substr(dotPos + 1) : "";

    // Get or create filesystem instance
    auto filesystem = repository_->getOrCreate(normalizedSource, extension);
    if (!filesystem)
        return false;

    // Add the mount point
    mounts.emplace_back(filesystem, normalizedSource, priority);
    eastl::sort(mounts.begin(), mounts.end());
    return true;
}

UnmountResult RootFilesystem::unmount(const String &mountPoint, const String &sourcePath, UnmountOption option)
{
    std::lock_guard lock(mountMutex_);

    auto normalizedMount = normalizePath(mountPoint);
    auto it              = mountPoints_.find(normalizedMount);
    if (it == mountPoints_.end())
        return UnmountResult::NotFound;

    auto &mounts = it->second;
    auto  mountIt =
        eastl::find_if(mounts.begin(), mounts.end(), [&](const MountPoint &mp) { return mp.sourcePath == sourcePath; });

    if (mountIt == mounts.end())
        return UnmountResult::NotFound;

    // Check for open files
    if (option == UnmountOption::FailIfFilesOpen && mountIt->filesystem->hasOpenFiles())
    {
        return UnmountResult::FilesInUse;
    }

    // Remove the mount point - this will release the shared_ptr reference
    // and the filesystem will be destroyed if no longer needed
    mounts.erase(mountIt);

    if (mounts.empty())
    {
        mountPoints_.erase(it);
    }

    return UnmountResult::Success;
}

eastl::vector<MountStatus> RootFilesystem::getMountStatus() const
{
    std::lock_guard lock(mountMutex_);

    eastl::vector<MountStatus> result;

    // Iterate through all mount points
    for (const auto &[mountPoint, mounts] : mountPoints_)
    {
        for (const auto &mount : mounts)
        {
            MountStatus status;
            status.mountPoint    = mountPoint;
            status.sourcePath    = mount.sourcePath;
            status.priority      = mount.priority;
            status.type          = mount.filesystem->getFilesystemType();
            status.openFileCount = mount.filesystem->hasOpenFiles() ? 1 : 0;

            result.push_back(eastl::move(status));
        }
    }

    return result;
}

FileHandle RootFilesystem::openFile(StringView path, IFile::OpenMode mode)
{
    String fullPath(path.data(), path.size());

    std::lock_guard lock(mountMutex_);
    auto            resolvedPaths = resolvePath(fullPath);

    // Try each mount point in priority order
    for (const auto &[fs, relativePath] : resolvedPaths)
    {
        auto fileHandle = fs->openFile(StringView(relativePath), mode);
        if (fileHandle)
        {
            return fileHandle;
        }
    }

    return nullptr;
}

FileStats RootFilesystem::stat(StringView path)
{
    String fullPath(path.data(), path.size());

    std::lock_guard lock(mountMutex_);
    auto            resolvedPaths = resolvePath(fullPath);

    // Try each mount point in priority order
    for (const auto &[fs, relativePath] : resolvedPaths)
    {
        auto stats = fs->stat(StringView(relativePath));
        if (stats.exists)
        {
            return stats;
        }
    }

    return FileStats{};
}

bool RootFilesystem::exists(StringView path)
{
    return stat(path).exists;
}

void RootFilesystem::visitEntries(StringView path, eastl::function<VisitResult(StringView, bool)> visitor)
{
    String fullPath(path.data(), path.size());

    std::lock_guard lock(mountMutex_);
    auto            resolvedPaths = resolvePath(fullPath);

    eastl::map<String, bool> seenEntries; // name -> is_dir

    // Visit entries from all mount points, respecting priority
    for (const auto &[fs, relativePath] : resolvedPaths)
    {
        fs->visitEntries(StringView(relativePath), [&](StringView name, bool isDir) {
            String nameStr(name.data(), name.size());
            if (seenEntries.find(nameStr) == seenEntries.end())
            {
                seenEntries[nameStr] = isDir;
                return visitor(name, isDir);
            }
            return VisitResult::VisitNext;
        });
    }
}

bool RootFilesystem::mkpath(StringView path)
{
    String fullPath(path.data(), path.size());

    std::lock_guard lock(mountMutex_);
    auto            resolvedPaths = resolvePath(fullPath);
    if (resolvedPaths.empty())
        return false;

    // Create directory in highest priority filesystem
    const auto &[fs, relativePath] = resolvedPaths.front();
    return fs->mkpath(StringView(relativePath));
}

bool RootFilesystem::hasOpenFiles() const
{
    std::lock_guard lock(mountMutex_);

    for (const auto &[_, mounts] : mountPoints_)
    {
        for (const auto &mount : mounts)
        {
            if (mount.filesystem->hasOpenFiles())
            {
                return true;
            }
        }
    }

    return false;
}

String RootFilesystem::resolveToNativePath(StringView path)
{
    String fullPath(path.data(), path.size());

    std::lock_guard lock(mountMutex_);
    auto            resolvedPaths = resolvePath(fullPath);

    for (const auto &[fs, relativePath] : resolvedPaths)
    {
        if (fs->convertableToNative() && fs->exists(relativePath))
        {
            String sourcePath = fs->getSourcePath();
            String result;
            // Join source path with relative path
            if (!sourcePath.empty() && sourcePath.back() != '/' && !relativePath.empty() && relativePath[0] != '/')
            {
                result = sourcePath + "/" + relativePath;
            }
            else
            {
                result = sourcePath + relativePath;
            }
            // Convert internal format to native: /C/path -> C:/path
            return PathUtils::externalizePath(result);
        }
    }
    return ""; // Not resolvable to native path
}

Vector<eastl::pair<eastl::shared_ptr<IFilesystem>, String>> RootFilesystem::resolvePath(const String &path)
{
    String                                                      normalizedPath = normalizePath(path);
    Vector<eastl::pair<eastl::shared_ptr<IFilesystem>, String>> result;

    // Find all matching mount points, longest prefix first
    Vector<eastl::pair<String, Vector<MountPoint>>> matchingMounts;
    for (const auto &[mountPoint, mounts] : mountPoints_)
    {
        if (normalizedPath.starts_with(mountPoint))
        {
            matchingMounts.emplace_back(mountPoint, mounts);
        }
    }

    // Sort by mount point length (longest first) for most specific match
    eastl::sort(matchingMounts.begin(), matchingMounts.end(),
                [](const auto &a, const auto &b) { return a.first.length() > b.first.length(); });

    if (!matchingMounts.empty())
    {
        const auto &[mountPoint, mounts] = matchingMounts.front();
        // Calculate skip length
        size_t skip_length = mountPoint.length();
        if (mountPoint.length() > 1 && mountPoint.ends_with('/'))
        {
            skip_length += 1; // Skip the separator '/'
        }

        String relativePath = ""; // Root directory of mounted filesystem
        if (skip_length < normalizedPath.length())
        {
            relativePath = normalizedPath.substr(skip_length);
        }
        // Add each filesystem in priority order
        for (const auto &mount : mounts)
        {
            result.emplace_back(mount.filesystem, relativePath);
        }
    }

    return result;
}

String RootFilesystem::normalizePath(const String &path)
{
    String normalized = path;

    // Replace backslashes with forward slashes
    eastl::replace(normalized.begin(), normalized.end(), '\\', '/');

    // Remove consecutive slashes
    auto it = eastl::unique(normalized.begin(), normalized.end(), [](char a, char b) { return a == '/' && b == '/'; });
    normalized.erase(it, normalized.end());

    // Handle "." and ".." components
    eastl::vector<String> components;
    String                component;

    // Add leading slash if present
    bool isAbsolute = !normalized.empty() && normalized[0] == '/';

    for (size_t i = isAbsolute ? 1 : 0; i < normalized.size(); ++i)
    {
        char c = normalized[i];
        if (c == '/')
        {
            if (component == ".")
            {
                // Skip "." component
            }
            else if (component == "..")
            {
                // Handle ".." by removing last component if possible
                if (!components.empty() && components.back() != "..")
                {
                    components.pop_back();
                }
                else if (!isAbsolute)
                {
                    // Only keep ".." in relative paths
                    components.push_back(component);
                }
            }
            else if (!component.empty())
            {
                components.push_back(component);
            }
            component.clear();
        }
        else
        {
            component += c;
        }
    }

    // Handle the last component
    if (component == ".")
    {
        // Skip
    }
    else if (component == "..")
    {
        if (!components.empty() && components.back() != "..")
        {
            components.pop_back();
        }
        else if (!isAbsolute)
        {
            components.push_back(component);
        }
    }
    else if (!component.empty())
    {
        components.push_back(component);
    }

    // Rebuild the path
    String result = isAbsolute ? "/" : "";
    for (size_t i = 0; i < components.size(); ++i)
    {
        if (i > 0)
            result += '/';
        result += components[i];
    }

    // Special case for empty path or just "."
    if (result.empty() && !isAbsolute)
    {
        result = ".";
    }

    return result;
}

// BaseFilesystem implementation
BaseFilesystem::BaseFilesystem(StringView sourcePath) : sourcePath_(sourcePath) {}

eastl::shared_ptr<RootFilesystem> createRootFilesystem(FilesystemFactory &&native_fs)
{
    auto rootFS = eastl::make_shared<RootFilesystem>();
    return rootFS;
}
} // namespace SEGS
