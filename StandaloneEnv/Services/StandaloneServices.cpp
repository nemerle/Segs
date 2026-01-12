#include "StandaloneServices.h"

#include "Common/Utils/IFilesystem.h"
#include "Common/Utils/IServiceLocator.h"
#include "Common/Utils/FilesystemHandler.h"
#include "Common/Utils/string_utils.h"

#include <QDirIterator>
#include <QFile>
#include <QString>

struct QFSHandle : public SEGS::IFile {
public:
    QFSHandle(const QString &path,OpenMode m=OpenMode::ReadOnly) {
        f = new QFile(path);
        reopen(m);
    }
    ~QFSHandle() {
        delete f;
    }
    bool    seek(int64_t offset, SeekOrigin origin=SeekOrigin::Begin) override {
        switch(origin) {

        case SEGS::IFile::Begin:
            return f->seek(offset);
        case SEGS::IFile::Current:
            return f->seek(offset+f->pos());
        case SEGS::IFile::End: break;
            return f->seek(f->size()-offset);
        }
        return false;

    }
    int64_t tell() override { return f->pos();}
    int64_t size() override { return f->size();}
    bool    atEnd() const override { return f->atEnd();}
    bool    flush() override { return f->flush();}
    int64_t read(void *data, int64_t maxlen) override {
        return f->read((char *)data,maxlen);
    }
    int64_t write(const void *data, int64_t len) override {
        return f->write((char *)data,len);
    }
    bool  reopen(OpenMode mode) override {
        QFile::OpenMode om;
        if(mode&OpenMode::ReadOnly)
            om|=QFile::ReadOnly;
        if(mode&OpenMode::WriteOnly)
            om|=QFile::WriteOnly;
        if(mode==OpenMode::Append)
            om|=QFile::Append;
        if(mode==OpenMode::Truncate)
            om|=QFile::Truncate;

        return f->open(om);
    }
    void    close() override { f->close();}
    bool    isOpen() const override { return f->isOpen(); }

    QFile *f;
};

// Helper to properly join base path and relative path with separator
// Returns path in native format for Qt operations
static String joinPath(const String& base, StringView relative) {
    String result = base;
    if (!result.empty() && result.back() != '/' && !relative.empty() && relative[0] != '/') {
        result += '/';
    }
    result.append(relative.data(), relative.size());
    // Convert internal format to native for Qt: /C/path -> C:/path
    return PathUtils::externalizePath(result);
}

struct QFSWrapper : public SEGS::BaseFilesystem
{
public:
    QFSWrapper(StringView sourcePath) : SEGS::BaseFilesystem(sourcePath) {}
    ~QFSWrapper() override = default;

    SEGS::FileHandle openFile(StringView path, SEGS::IFile::OpenMode mode) override;
    SEGS::FileStats stat(StringView path) override;
    bool exists(StringView path) override;
    void visitEntries(StringView                                                      path,
                      eastl::function<SEGS::VisitResult(StringView, bool /*is_dir*/)> visitor) override;

    bool            mkpath(StringView path) override;
    String getFilesystemType() const override { return "NativeFilesystem"; }
    bool convertableToNative() const override { return true; }
};
bool QFSWrapper::mkpath(StringView path)
{
    auto full_path = joinPath(getSourcePath(), path);
    QString q_path = QString::fromUtf8(full_path.data(), full_path.size());
    return QDir(q_path).mkpath(".");
}

SEGS::FileStats QFSWrapper::stat(StringView path)
{
    SEGS::FileStats res;
    auto full_path = joinPath(getSourcePath(), path);
    QString         q_path = QString::fromUtf8(full_path.data(), full_path.size());
    QFileInfo       fi(q_path);
    res.size          = fi.size();
    res.exists        = fi.exists();
    res.is_dir        = fi.isDir();
    res.last_modified = fi.lastModified().toMSecsSinceEpoch();
    return res;
}

void QFSWrapper::visitEntries(StringView path, eastl::function<SEGS::VisitResult(StringView, bool /*is_dir*/)> visitor)
{
    QString     q_path = QString::fromUtf8(path.data(), path.size());
    QStringList to_visit;
    // Convert internal format to native for Qt
    String nativeSourcePath = PathUtils::externalizePath(getSourcePath());
    QString basepath = QString::fromUtf8(nativeSourcePath.c_str());
    if (!basepath.isEmpty() && !basepath.endsWith('/')) {
        basepath += '/';
    }
    to_visit.push_back(q_path);
    while (!to_visit.empty())
    {
        QDirIterator iter(basepath + to_visit.takeFirst());
        while (iter.hasNext())
        {
            QString    fpath    = iter.next();
            QFileInfo  fi(fpath);
            QString    name     = fi.fileName();
            QByteArray name_utf8 = name.toUtf8();
            auto       vr = visitor(StringView(name_utf8.data(), name_utf8.size()), fi.isDir());
            switch (vr)
            {
            case SEGS::VisitResult::VisitNext: continue;
            case SEGS::VisitResult::VisitSubdirectory: to_visit.push_back(fpath); break;
            case SEGS::VisitResult::VisitStop: return;
            }
        }
    }
}

bool QFSWrapper::exists(StringView path)
{
    auto full_path = joinPath(getSourcePath(), path);
    QString q_path = QString::fromUtf8(full_path.data(), full_path.size());

    return QFile::exists(q_path);
}

SEGS::FileHandle QFSWrapper::openFile(StringView path, SEGS::IFile::OpenMode mode)
{
    if (path.empty() == 0)
    {
        return nullptr;
    }
    auto full_path = joinPath(getSourcePath(), path);
    QString q_path = QString::fromUtf8(full_path.data(), full_path.size());
    if (!QFile::exists(q_path) && mode == SEGS::IFile::OpenMode::ReadOnly)
    {
        return nullptr;
    }
    auto ptr=new QFSHandle(q_path);
    if (!ptr->isOpen())
    {
        delete ptr;
        return nullptr;
    }
    return wrapFile(ptr);
}

struct LoggerWrapper : public SEGS::ILogger
{
public:
    void logString(int log_level, const char *debug_msg) override {
        switch(static_cast<SegsLogLevel>(log_level))
        {
        case SegsLogLevel::Debug:
            qDebug() << debug_msg;
            break;
        case SegsLogLevel::Info:
            qInfo() << debug_msg;
            break;
        case SegsLogLevel::Warning:
            qWarning() << debug_msg;
            break;
        case SegsLogLevel::Error:
            qCritical() << debug_msg;
            break;
        case SegsLogLevel::Unknown:
            qDebug()<<debug_msg;
            break;
        }
    }
};

struct QCompressor : public SEGS::ICompressionService {

public:
    DecompressionResult *uncompressZip(const char *compressed_data, int compressed_size, int decompressed_size) override
    {
        if(compressed_size==0)
            return nullptr;
        QByteArray cd(compressed_data,compressed_size);
        cd.prepend( char((decompressed_size >> 0) & 0xFF));
        cd.prepend( char((decompressed_size >> 8) & 0xFF));
        cd.prepend( char((decompressed_size >> 16) & 0xFF));
        cd.prepend( char((decompressed_size >> 24) & 0xFF));
        auto *result=new DecompressionResult;
        // this is sad, but we can't take the memory from QByteArray to return it directly. :(
        QByteArray decomp=qUncompress(compressed_data);
        auto data = new char[decomp.size()];
        memcpy(data,decomp.constData(),decomp.size());
        result->data = data;
        result->size = decomp.size();
        return result;
    }
    CompressionResult *compressData(const char *data, int data_size) override
    {
        QByteArray ba = qCompress(reinterpret_cast<const uint8_t *>(data),data_size,5);
        ba.remove(0,sizeof(uint32_t)); // qt includes uncompressed size as a first 4 bytes of QByteArray
        auto *compressed = new char[ba.size()];
        memcpy(compressed,ba.data(),ba.size());
        auto *res=new CompressionResult;
        res->data=compressed;
        res->size=ba.size();
        return res;
    }
};

class StandaloneServiceLocator : public SEGS::BaseServiceLocator {
public:
    StandaloneServiceLocator(const String &basepath);
    SEGS::ICompressionService *getCompression() override { return &compr; }
    SEGS::ILogger             *getLogger() override { return &logger; }
private:
    QCompressor compr;
    LoggerWrapper logger;
};

void registerEnvSingleton() {
    String app_dir = QDir::currentPath().toUtf8().constData();
    static StandaloneServiceLocator locator(app_dir);
    SEGS::setServiceLocator(&locator);
}
static SEGS::FilesystemFactory getNativeFSFactory() {
    return [](StringView path)->auto { return eastl::make_shared<::QFSWrapper>(path); };
}

StandaloneServiceLocator::StandaloneServiceLocator(const String &app_dir) : SEGS::BaseServiceLocator(eastl::move(getNativeFSFactory()), app_dir)  {

}


