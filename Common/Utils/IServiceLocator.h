#pragma once
#include <stdint.h>
#include "FilesystemHandler.h"
#include "EASTL/shared_ptr.h"

namespace SEGS {

class RootFilesystem;

class ICompressionService {
public:
    struct DecompressionResult {
        char *data;
        uint32_t size;
        ~DecompressionResult() { delete[] data; }
    };
    struct CompressionResult {
        char *data;
        uint32_t size;
        ~CompressionResult() { delete[] data; }
    };
    virtual DecompressionResult *uncompressZip(const char *compressed_data,int compressed_size,int decompressed_size)=0;
    // compress
    virtual CompressionResult *compressData(const char *data,int data_size)=0;
};

struct ILogger {
    enum SegsLogLevel : uint8_t {
        Debug=0,
        Info=1,
        Warning=2,
        Error=3,
        Unknown=4
    };
    virtual void logString(int log_level,const char *debug_msg)=0;
};

class IServiceLocator {
public:
    virtual ~IServiceLocator() {}
    virtual ICompressionService *getCompression()=0;
    virtual RootFilesystem *getFS()=0;
    virtual ILogger *getLogger()=0;
};

class BaseServiceLocator : public IServiceLocator {
public:
    BaseServiceLocator(FilesystemFactory &&native_fs, const String& app_dir = "");
    ~BaseServiceLocator() override;
    RootFilesystem *getFS() final { return m_fs.get(); }
protected:
    eastl::unique_ptr<RootFilesystem> m_fs;
};

IServiceLocator *getServiceLocator();
void setServiceLocator(IServiceLocator *sl);

inline ICompressionService *getCompressionService() {
    IServiceLocator * sl = getServiceLocator();
    return sl ? sl->getCompression() : nullptr;
}

}
