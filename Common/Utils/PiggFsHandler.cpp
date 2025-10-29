/*
 * Pigg Filesystem Implementation
 * Complete .cpp file with corrected reference counting
 */

#include "PiggFsHandler.h"
#include "Common/Containers/Vector.h"
#include "Utils/IServiceLocator.h"
#include "Common/Containers/Vector.h"
#include "Common/Containers/Map.h"

#include "EASTL/memory.h"
#include "EASTL/set.h"
#include "EASTL/utility.h"
#include <cassert>

namespace {
using namespace SEGS;

#pragma pack(push, 1)
struct PiggHeader {
    int pigg_magic;
    int16_t unused;
    int16_t version;
    int16_t header_size;
    int16_t used_header_bytes;
    uint32_t num_entries;
};

struct PiggInternalHeader {
    int flag;
    int name_id;
    uint32_t size;
    uint32_t ftime;
    uint32_t offset;
    int unused[6];
    uint32_t packed_size;
};
#pragma pack(pop)

struct PiggDataTable {
    int datapool_flag;
    eastl::vector<eastl::string> data_parts;
};



// =============================================================================
// PIGG FILESYSTEM IMPLEMENTATION
// =============================================================================

class PiggFilesystem : public BaseFilesystem {
public:
    explicit PiggFilesystem(StringView archive_path);
    ~PiggFilesystem() override;

           // IFilesystem interface implementation
    FileHandle openFile(StringView path, IFile::OpenMode mode) override;
    FileStats stat(StringView path) override;
    bool exists(StringView path) override;
    void visitEntries(StringView path, eastl::function<VisitResult(StringView, bool)> visitor) override;
    bool mkpath(StringView path) override;
    eastl::string getFilesystemType() const override { return "PiggFilesystem"; }
    eastl::string getSourcePath() const override { return sourcePath_; }

private:
    SEGS::FileHandle archive_file_;
    PiggHeader header_;
    eastl::vector<PiggInternalHeader> file_headers_;
    PiggDataTable strings_table_;
    PiggDataTable headers_table_;
    eastl::map<eastl::string, size_t> file_index_;
    bool is_loaded_ = false;

    bool loadPiggArchive();
    bool loadDataTable(SEGS::FileHandle& src, PiggDataTable& target);
    void notifyFileOpened() override {}
    void notifyFileClosed() override {}

};

// =============================================================================
// PIGG FILE IMPLEMENTATION
// =============================================================================

class PiggFile : public IFile {
public:
    PiggFile(IFile *archive_file, const PiggInternalHeader& header,
             bool is_compressed, uint64_t data_offset);
    ~PiggFile() override;

           // IFile interface implementation
    int64_t read(void* buffer, int64_t size) override;
    int64_t write(const void* buffer, int64_t size) override;
    bool seek(int64_t offset, SeekOrigin origin) override;
    int64_t tell() override;
    int64_t size() override;
    bool flush() override;
    bool isOpen() const override;
    void close() override;
    bool atEnd() const override;
    bool reopen(OpenMode) override { is_open_=archive_file_->isOpen(); return is_open_; }
private:
    IFile * archive_file_;
    PiggInternalHeader header_;
    bool is_compressed_;
    uint64_t data_offset_;
    Vector<uint8_t> file_data_;
    int64_t position_ = 0;
    bool is_open_ = true;

    bool extractFileData();

};

// Helper function to read POD structures from QIODevice
template<class POD>
bool readPOD(IFile *src, POD& tgt) {
    return sizeof(POD) == src->read((char*)&tgt, sizeof(POD));
}

// =============================================================================
// PIGG FILE IMPLEMENTATION
// =============================================================================

PiggFile::PiggFile(IFile * archive_file, const PiggInternalHeader& header,
                   bool is_compressed, uint64_t data_offset)
    : archive_file_(archive_file)
      , header_(header)
      , is_compressed_(is_compressed)
      , data_offset_(data_offset)
{
    extractFileData();
    is_open_ = archive_file->isOpen();
}

PiggFile::~PiggFile() {
    close();
}

bool PiggFile::extractFileData() {
    if (!archive_file_ || !archive_file_->isOpen()) {
        return false;
    }

           // Save current position and seek to file data
    int64_t current_pos = archive_file_->tell();
    if (!archive_file_->seek(data_offset_)) {
        return false;
    }

           // Read the file data
    Vector<uint8_t> compressed_data;
    if (is_compressed_) {
        compressed_data.resize(header_.packed_size);
        auto read_count = archive_file_->read(compressed_data.data(),header_.packed_size);
        if (read_count != static_cast<int>(header_.packed_size)) {
            archive_file_->seek(current_pos);
            return false;
        }
    } else {
        file_data_.resize(header_.size);
        auto read_count= archive_file_->read(file_data_.data(),header_.size);
        if (read_count != static_cast<int>(header_.size)) {
            archive_file_->seek(current_pos);
            return false;
        }
    }

           // Restore position
    archive_file_->seek(current_pos);

           // Decompress if needed
    if (is_compressed_ && !compressed_data.empty()) {
        auto * dec = getServiceLocator()->getCompression()->uncompressZip((const char *)compressed_data.data(),compressed_data.size(),header_.size);
        if(dec) {
            file_data_.assign(dec->data,dec->data+dec->size);
            delete dec;
        }

               // Check if decompression was successful
        if (file_data_.size() != static_cast<int>(header_.size)) {
            file_data_.clear();
            return false;
        }
    }

    return !file_data_.empty();
}

int64_t PiggFile::read(void* buffer, int64_t size) {
    if (!is_open_ || file_data_.empty() || position_ >= file_data_.size()) {
        return 0;
    }

           // Calculate how many bytes we can actually read
    int64_t bytes_available = static_cast<int64_t>(file_data_.size() - position_);
    int64_t bytes_to_read = eastl::min(bytes_available, size);

           // Copy data to the provided buffer
    memcpy(buffer, file_data_.data() + position_, bytes_to_read);
    position_ += static_cast<int64_t>(bytes_to_read);

    return bytes_to_read;
}

int64_t PiggFile::write(const void* buffer, int64_t size) {
    // Writing is not supported for pigg archives
    (void)buffer; // Suppress unused parameter warning
    (void)size;
    getServiceLocator()->getLogger()->logString(ILogger::Warning,"Attempt to write to pigg file, not supported");
    return 0;
}

bool PiggFile::seek(int64_t offset, SeekOrigin origin) {
    if (!is_open_ || file_data_.empty()) {
        return false;
    }

    int64_t new_position = 0;
    int64_t file_size = static_cast<int64_t>(file_data_.size());

    switch (origin) {
    case Begin:
        new_position = offset;
        break;
    case Current:
        new_position = position_ + offset;
        break;
    case End:
        new_position = file_size + offset;
        break;
    }

           // Check bounds
    if (new_position < 0 || new_position > file_size) {
        return false;
    }

    position_ = new_position;
    return true;
}

int64_t PiggFile::tell() {
    if (!is_open_) {
        return -1;
    }

    return position_;
}

int64_t PiggFile::size() {
    if (!is_open_ || file_data_.empty()) {
        return -1;
    }

    return static_cast<int64_t>(file_data_.size());
}

bool PiggFile::flush() {
    // No write support, so flush does nothing but should return true if file is open
    return is_open_;
}

bool PiggFile::isOpen() const {
    return is_open_;
}

void PiggFile::close() {
    is_open_ = false;
    // We keep the file_data_ to avoid re-extraction if the file is used again
}

bool PiggFile::atEnd() const {
    return position_>=static_cast<int64_t>(file_data_.size());
}

// =============================================================================
// PIGG FILESYSTEM IMPLEMENTATION
// =============================================================================

PiggFilesystem::PiggFilesystem(StringView archive_path) : BaseFilesystem(String(archive_path))
{
    is_loaded_ = loadPiggArchive();
}

PiggFilesystem::~PiggFilesystem() {
    if (archive_file_ && archive_file_->isOpen()) {
        archive_file_->close();
    }
}

bool PiggFilesystem::loadPiggArchive() {
    auto *root_fs = getServiceLocator()->getFS();
    assert(root_fs);
    archive_file_ = root_fs->openFile(sourcePath_);

    if (!archive_file_->isOpen()) {
        return false;
    }

           // Read pigg header
    if (!readPOD(archive_file_.get(), header_)) {
        return false;
    }

           // Validate magic number
    if (header_.pigg_magic != 0x123) {
        return false;
    }

           // Check version
    if (header_.version > 2) {
        return false;
    }

           // Skip any extra header bytes
    if (header_.header_size != 0x10) {
        archive_file_->seek(archive_file_->tell() + header_.header_size - 0x10);
    }

           // Read file headers
    file_headers_.resize(header_.num_entries);

    for (auto& in_hdr : file_headers_) {
        if (!readPOD(archive_file_.get(), in_hdr)) {
            return false;
        }

               // Validate header flag
        if (in_hdr.flag != 0x3456) {
            return false;
        }

               // Check for valid size
        uint32_t internal_fsize = in_hdr.packed_size ? in_hdr.packed_size : in_hdr.size;
        if (in_hdr.offset + internal_fsize > static_cast<uint32_t>(archive_file_->size())) {
            return false;
        }

               // Check for valid name id
        if (in_hdr.name_id < 0) {
            return false;
        }
    }

           // Load string table
    if (!loadDataTable(archive_file_, strings_table_)) {
        return false;
    }

           // Load headers table (not used directly but needed to advance file pointer)
    if (!loadDataTable(archive_file_, headers_table_)) {
        return false;
    }

           // Validate table magic
    if (strings_table_.datapool_flag != 0x6789 || headers_table_.datapool_flag != 0x9ABC) {
        return false;
    }

           // Validate that we have enough strings for all file headers
    if (strings_table_.data_parts.empty()) {
        return false;
    }

           // Find maximum name_id to validate bounds
    int max_name_id = 0;
    for (const auto& header : file_headers_) {
        if (header.name_id > max_name_id) {
            max_name_id = header.name_id;
        }
    }

    if (max_name_id >= static_cast<int>(strings_table_.data_parts.size())) {
        return false;
    }

           // Build file index map for faster lookups
    for (size_t i = 0; i < file_headers_.size(); ++i) {
        const auto& header = file_headers_[i];
        if (header.name_id >= 0 && header.name_id < static_cast<int>(strings_table_.data_parts.size())) {
            auto filename = strings_table_.data_parts[header.name_id];
            file_index_[filename] = i;
        }
    }

    return true;
}

bool PiggFilesystem::loadDataTable(SEGS::FileHandle& src, PiggDataTable& target) {
    int table_sizes;
    int num_entries;

    if (!readPOD(src.get(), target.datapool_flag)) {
        return false;
    }

    if (!readPOD(src.get(), table_sizes)) {
        return false;
    }

    if (!readPOD(src.get(), num_entries)) {
        return false;
    }

           // Prevent excessive memory allocation
    if (num_entries < 0 || num_entries > 10000000) {
        return false;
    }

    while (num_entries > 0) {
        uint32_t entrysize;
        if (!readPOD(src.get(), entrysize)) {
            return false;
        }

               // Prevent excessive memory allocation for individual entries
        if (entrysize > 100000) {
            return false;
        }

        if (entrysize > 0) {
            Vector<uint8_t> data;
            data.resize(entrysize);
            auto read_size= src->read(data.data(),entrysize);
            if (read_size != static_cast<int>(entrysize)) {
                return false;
            }
            target.data_parts.emplace_back((char *)data.data());
        }

        num_entries -= static_cast<int>(entrysize + 4);
    }

    return true;
}

FileHandle PiggFilesystem::openFile(StringView path, IFile::OpenMode mode) {
    if (!is_loaded_ || mode != IFile::ReadOnly) {
        return nullptr;
    }

    String path_str(path.data(), path.size());

           // Look up the file in our index
    auto it = file_index_.find(path_str);
    if (it == file_index_.end()) {
        return nullptr;
    }

           // Get the file header
    const auto& file_hdr = file_headers_[it->second];
    bool is_compressed = file_hdr.packed_size != 0;

           // Create the file
    auto pigg_file = new PiggFile(
        archive_file_.get(),
        file_hdr,
        is_compressed,
        file_hdr.offset
        );

           // Check if file creation was successful
    if (!pigg_file->isOpen()) {
        delete pigg_file;
        return nullptr;
    }

           // Wrap with our helper method
    return wrapFile(pigg_file);
}

FileStats PiggFilesystem::stat(StringView path) {
    FileStats stats;

    if (!is_loaded_) {
        return stats;
    }

    String path_str(path.data(), path.size());

           // Look up the file in our index
    auto it = file_index_.find(path_str);
    if (it == file_index_.end()) {
        return stats;
    }

           // Get the file header
    const auto& file_hdr = file_headers_[it->second];

           // Fill in stats
    stats.exists = true;
    stats.is_dir = false;
    stats.size = static_cast<int64_t>(file_hdr.size);
    stats.last_modified = static_cast<int64_t>(file_hdr.ftime);

    return stats;
}

bool PiggFilesystem::exists(StringView path) {
    if (!is_loaded_) {
        return false;
    }

    String path_str(path.data(), path.size());
    return file_index_.find(path_str) != file_index_.end();
}

void PiggFilesystem::visitEntries(StringView path, eastl::function<VisitResult(StringView, bool)> visitor) {
    if (!is_loaded_) {
        return;
    }

    String path_str(path.data(), path.size());

           // Handle the root directory
    if (path_str.empty() || path_str == "/" || path_str == ".") {
        // Create sets to avoid duplicates and ensure consistent ordering
        eastl::set<String> directories;
        eastl::set<String> files;

        for (const auto& entry : file_index_) {
            const String& filename = entry.first;

                   // Check if this file contains path separators
            size_t slash_pos = filename.find('/');
            if (slash_pos != String::npos) {
                // Extract the first directory level
                String dir_name = filename.substr(0, slash_pos);
                directories.insert(dir_name);
            } else {
                // This is a file in the root directory
                files.insert(filename);
            }
        }

               // Visit all files first
        for (const auto& file : files) {
            VisitResult result = visitor(StringView(file), false);
            if (result == VisitResult::VisitStop) {
                return;
            }
        }

               // Visit all directories
        for (const auto& dir : directories) {
            VisitResult result = visitor(StringView(dir), true);
            if (result == VisitResult::VisitStop) {
                return;
            }
        }

        return;
    }

           // Normalize path for subdirectory processing
    String normalized_path = path_str;
    if (normalized_path.back() != '/') {
        normalized_path += '/';
    }

           // Find all entries that start with this path
    eastl::set<String> subdirectories;
    eastl::set<String> files;

    for (const auto& entry : file_index_) {
        const String& filename = entry.first;

        if (filename.find(normalized_path) == 0) {
            // Extract the part after the path
            String relative = filename.substr(normalized_path.length());

                   // Skip empty relative paths
            if (relative.empty()) {
                continue;
            }

                   // Check if this is a file or a subdirectory
            size_t slash_pos = relative.find('/');
            if (slash_pos != String::npos) {
                // This is a subdirectory, extract just the directory name
                String dir_name = relative.substr(0, slash_pos);
                subdirectories.insert(dir_name);
            } else {
                // This is a file
                files.insert(relative);
            }
        }
    }

           // Visit all files first
    for (const auto& file : files) {
        VisitResult result = visitor(StringView(file), false);
        if (result == VisitResult::VisitStop) {
            return;
        }
    }

           // Visit all subdirectories
    for (const auto& dir : subdirectories) {
        VisitResult result = visitor(StringView(dir), true);
        if (result == VisitResult::VisitStop) {
            return;
        }
    }
}

bool PiggFilesystem::mkpath(StringView path) {
    // Pigg archives are read-only, so we can't create directories
    (void)path; // Suppress unused parameter warning
    return false;
}


}

namespace SEGS {
// =============================================================================
// FACTORY FUNCTIONS
// =============================================================================

// Factory function for creating pigg filesystems
FilesystemFactory createPiggFilesystemFactory() {
    return [](StringView path) -> eastl::shared_ptr<IFilesystem> {
        return eastl::make_shared<PiggFilesystem>(path);
    };
}
}
