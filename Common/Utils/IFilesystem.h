#pragma once

#include "Common/Containers/StringView.h"
#include "Common/Utils/IFile.h"
#include <EASTL/functional.h>
#include <EASTL/shared_ptr.h>
#include <magic_enum/magic_enum.hpp>
#include <stdint.h>

// View of a string (to avoid unnecessary copying)
using StringView  = eastl::string_view;


namespace SEGS {

struct FileStats {
    uint8_t exists:1=false;
    uint8_t is_dir:1=false;
    int64_t size;
    int64_t last_modified;
};
enum class VisitResult {
    VisitNext=0,
    VisitSubdirectory=1,
    VisitStop=2
};

using FileHandle = eastl::unique_ptr<IFile, eastl::function<void(IFile*)>>;


// A simple file access wrapper to allow re-locating/packing files
class IFilesystem  : public eastl::enable_shared_from_this<IFilesystem> {
public:

    virtual ~IFilesystem() = default;
    virtual FileHandle openFile(StringView path, IFile::OpenMode mode=IFile::ReadOnly) = 0;
    virtual FileStats stat(StringView path) = 0;
    virtual bool exists(StringView path) = 0;
    virtual void visitEntries(StringView path, eastl::function<VisitResult(StringView, bool /*is_dir*/)> visitor) = 0;
    virtual bool mkpath(StringView path) = 0;
    // For debugging/introspection
    virtual eastl::string getFilesystemType() const = 0;
    // Returns the source path of the filesystem, e.g. where the files are located (root directory)
    virtual eastl::string getSourcePath() const = 0;

    // Simple manual counting
    virtual bool hasOpenFiles() const = 0;
    // Returns true if paths can be resolved to native filesystem paths
    // Archive filesystems (pigg, zip) return false since files are inside archives
    virtual bool convertableToNative() const { return false; }
protected:
    // Helper method for subclasses to create FileHandle with proper cleanup
    FileHandle wrapFile(IFile* file) {
        if (!file || !file->isOpen()) {
            delete file;
            return nullptr;
        }
        notifyFileOpened();
        // Capture shared_ptr to keep filesystem alive while file is open
        auto shared_self = shared_from_this();
        return FileHandle(file, [shared_self](IFile* f) {
            shared_self->notifyFileClosed();
            delete f;  // shared_self destructor keeps filesystem alive
        });
    }
    virtual void notifyFileOpened() {}
    virtual void notifyFileClosed() {}
};

}
