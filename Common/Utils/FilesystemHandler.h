#pragma once
#include "Common/Utils/IFilesystem.h"

#include "EASTL/map.h"
#include <mutex>
// View of a string (to avoid unnecessary copying)
using StringView  = eastl::string_view;
using String = eastl::string;

namespace SEGS {
// Enum for unmount result
enum class UnmountResult {
    Success,
    FilesInUse,
    NotFound
};

// Enum for unmount options
enum class UnmountOption {
    FailIfFilesOpen,    // Fail if files are open (default)
    ForceClose          // Force unmount even with open files
};

// Mount status information
struct MountStatus {
    String mountPoint;
    String sourcePath;
    int priority;
    int openFileCount;
    String type;
};

// Factory function type for creating filesystem implementations
using FilesystemFactory = eastl::function<eastl::shared_ptr<IFilesystem>(StringView path)>;

// Structure to represent a mounted filesystem with priority
struct MountPoint {
    eastl::shared_ptr<IFilesystem> filesystem;
    String sourcePath;
    int priority;

    MountPoint(eastl::shared_ptr<IFilesystem> fs, String source, int prio)
        : filesystem(eastl::move(fs))
          , sourcePath(eastl::move(source))
          , priority(prio) {}

    bool operator<(const MountPoint& other) const {
        return priority > other.priority; // Higher priority first
    }
};

// Filesystem repository for managing shared instances
class FilesystemRepository {
public:
    // Get an existing filesystem or create a new one
    eastl::shared_ptr<IFilesystem> getOrCreate(const eastl::string& path, const eastl::string& extension);

           // Register a factory for a specific file extension
    void registerFactory(const eastl::string& extension, FilesystemFactory factory);

           // Get the number of active filesystem instances
    size_t getActiveInstanceCount() const;

private:
    mutable std::mutex mutex_;
    eastl::map<eastl::string, eastl::weak_ptr<IFilesystem>> filesystems_;
    eastl::map<eastl::string, FilesystemFactory> factories_;

           // Create a new filesystem instance based on extension
    eastl::shared_ptr<IFilesystem> createFilesystem(const eastl::string& path, const eastl::string& extension);

           // Get file extension from path
    eastl::string getFileExtension(const eastl::string& path);
};

// Root filesystem implementation that manages mounting
class RootFilesystem final : public IFilesystem {
public:
    RootFilesystem();
    ~RootFilesystem() override;;

           // Register a factory for a specific file extension
    void registerFactory(const String& extension, FilesystemFactory factory);

           // Mount a filesystem at the specified path with priority
    bool mount(const String& sourcePath, const String& mountPoint, int priority);

           // Unmount a filesystem
    UnmountResult unmount(const String& mountPoint, const String& sourcePath,
                          UnmountOption option = UnmountOption::FailIfFilesOpen);

           // Get mount status information
    eastl::vector<MountStatus> getMountStatus() const;

           // IFilesystem interface implementation
    FileHandle openFile(StringView path, IFile::OpenMode mode=IFile::OpenMode::ReadOnly) override;
    FileStats stat(StringView path) override;
    bool exists(StringView path) override;
    void visitEntries(StringView path, eastl::function<VisitResult(StringView, bool)> visitor) override;
    bool mkpath(StringView path) override;
    bool hasOpenFiles() const override;

    // Resolve a virtual path to a native filesystem path
    // Returns empty string if the path resolves to an archive filesystem
    String resolveToNativePath(StringView path);
    String getFilesystemType() const override { return "RootFilesystem"; }
    String getSourcePath() const override { return "root"; }

private:
    void notifyFileOpened() override {
        // shouldn't be called
    }

    void notifyFileClosed() override {
        // shouldn't be called
    }
    mutable std::recursive_mutex mountMutex_;
    eastl::shared_ptr<FilesystemRepository> repository_;
    eastl::map<String, eastl::vector<MountPoint>> mountPoints_;

           // Helper methods
    eastl::vector<eastl::pair<eastl::shared_ptr<IFilesystem>, String>> resolvePath(const String& path);
    static String normalizePath(const String& path);

};

// Base filesystem implementation with common functionality
class BaseFilesystem : public IFilesystem {
public:
    explicit BaseFilesystem(StringView sourcePath);
    ~BaseFilesystem() override = default;

           // Common implementation of hasOpenFiles
    bool hasOpenFiles() const override {
        return open_file_count_ > 0;  // Simple and reliable
    }

    String getSourcePath() const override { return sourcePath_; }
protected:
    String sourcePath_;
    mutable std::mutex fileMutex_;
    eastl::atomic<int> open_file_count_{0};

    void notifyFileOpened() override {
        ++open_file_count_;
    }

    void notifyFileClosed() override {
        --open_file_count_;
    }
};

// Native filesystem implementation for directories
// class NativeFilesystem : public BaseFilesystem {
// public:
//     explicit NativeFilesystem(const String& basePath);
//     ~NativeFilesystem() override = default;

//            // IFilesystem interface implementation
//     FileHandle openFile(StringView path, IFile::OpenMode mode) override;
//     FileStats stat(StringView path) override;
//     bool exists(StringView path) override;
//     void visitEntries(StringView path, eastl::function<VisitResult(StringView, bool)> visitor) override;
//     bool mkpath(StringView path) override;
//     String getFilesystemType() const override { return "NativeFilesystem"; }

// private:
//     String combinePath(const String& relPath);
// };
}
