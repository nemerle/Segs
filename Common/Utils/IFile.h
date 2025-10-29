#pragma once

#include <cstdlib>
#include "Common/Containers/Vector.h"

namespace SEGS
{
// Interface for file access operations
struct IFile
{
    enum OpenMode
    {
        None       = 0x0000,
        ReadOnly   = 1,                    // Open file for reading only
        WriteOnly  = 2,                    // Open file for writing only (creates if doesn't exist)
        ReadWrite  = ReadOnly | WriteOnly, // Open file for reading and writing (creates if doesn't exist)
        Append     = 0x0004,
        Truncate   = 0x0008,
        Text       = 0x0010,
        Unbuffered = 0x0020
    };
    // Seek origin positions
    enum SeekOrigin
    {
        Begin,   // Start of file
        Current, // Current position
        End      // End of file
    };
    virtual ~IFile() = default;
    virtual void close() = 0;
    virtual bool reopen(OpenMode mode) = 0;
    virtual bool isOpen() const = 0;
    // File operations

    // Read data from file into buffer
    // Returns number of bytes read
    virtual int64_t read(void *buffer, int64_t size) = 0;

    // Write data from buffer to file
    // Returns number of bytes written
    virtual int64_t write(const void *buffer, int64_t size) = 0;

    // Set file position
    // Returns true if successful
    virtual bool seek(int64_t offset, SeekOrigin origin=SeekOrigin::Begin) = 0;

    // Get current file position
    // Returns -1 on error
    virtual int64_t tell() = 0;

    // Get file size
    // Returns -1 on error
    virtual int64_t size() = 0;

    virtual bool atEnd() const = 0;

    // Flush file buffers to disk
    // Returns true if successful
    virtual bool flush() = 0;

    // Utility function to read entire file
    template <typename Vector = Vector<char>>
    Vector readAll()
    {
        Vector  res;
        int64_t maxlen = size();
        for (int64_t read_bytes = 0; read_bytes < maxlen; read_bytes += 1024)
        {
            int64_t to_read = maxlen - read_bytes;
            if (to_read > 1024)
                to_read = 1024;
            Vector  chunk(to_read);
            int64_t chunk_read = read(chunk.data(), to_read);
            if (chunk_read <= 0)
                break;
            res.insert(res.end(), chunk.begin(), chunk.begin() + chunk_read);
        }
        return res;
    }
};

} // namespace SEGS
