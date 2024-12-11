#include "StandaloneServices.h"

#include "Common/Utils/IServiceLocator.h"

#include <QDirIterator>
#include <QFile>
#include <QString>

struct QFSHandle : public SEGS::IFile {
public:
    QFSHandle(const QString &path) {
        f = new QFile(path);
    }
    ~QFSHandle() {
        delete f;
    }
    virtual bool    seek(int64_t pos) { return f->seek(pos);}
    virtual int64_t pos() const { return f->pos();}
    virtual int64_t size() const { return f->size();}
    virtual bool    atEnd() const { return f->atEnd();}
    virtual bool    reset() { return f->reset();}
    virtual bool    flush() { return f->flush();}
    virtual int64_t read(char *data, int64_t maxlen) {
        return f->read(data,maxlen);
    }
    virtual int64_t write(const char *data, int64_t len) {
        return f->write(data,len);
    }
    virtual bool    isSequential() const {
        return f->isSequential();
    }
    virtual bool    open(OpenMode mode) {
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
    virtual void    close() { f->close();}
    virtual bool    isOpen() const { return f->isOpen(); }

    QFile *f;
};

struct QFSWrapper : public SEGS::IFilesystem
{
public:
    ~QFSWrapper() override = default;

    SEGS::IFile *open(StringView path, SEGS::IFile::OpenMode mode) override {
        if(path.empty()==0)
        {
            return nullptr;
        }
        QString q_path = QString::fromUtf8(path.data(),path.size());
        if(!QFile::exists(q_path) && mode==SEGS::IFile::OpenMode::ReadOnly) {
            return nullptr;
        }
        auto res=new QFSHandle(q_path);
        if(!res->open(mode)) {
            delete res;
            return nullptr;
        }
        return res;
    }

    bool exists(StringView path) override {
        return QFile::exists(QString::fromUtf8(path.data(),path.size()));
    }

    void visitEntries(StringView path, eastl::function<VisitResult(StringView, bool /*is_dir*/)> visitor) override {
        QString q_path = QString::fromUtf8(path.data(),path.size());
        QStringList to_visit;
        to_visit.push_back(q_path);
        while(!to_visit.empty()) {
            QDirIterator iter(to_visit.takeFirst());
            while(iter.hasNext()) {

                QString fpath = iter.next();
                QByteArray path_utf8=fpath.toUtf8();
                QFileInfo fi(fpath);
                auto vr=visitor(StringView(path_utf8.data(),path_utf8.size()),fi.isDir());
                switch(vr) {
                case SEGS::IFilesystem::VisitNext:
                    continue;
                case SEGS::IFilesystem::VisitSubdirectory:
                    to_visit.push_back(fpath); break;
                case SEGS::IFilesystem::VisitStop:
                    return;
                }
            }
        }
    }

    SEGS::FileStats stat(StringView path) override {
        SEGS::FileStats res;
        QString q_path = QString::fromUtf8(path.data(),path.size());
        QFileInfo fi(q_path);
        res.size = fi.size();
        res.exists = fi.exists();
        res.is_dir = fi.isDir();
        res.last_modified = fi.lastModified().toMSecsSinceEpoch();
        return res;
    }
    bool mkpath(StringView path) override {
        QString q_path = QString::fromUtf8(path.data(),path.size());
        return QDir(q_path).mkpath(".");
    }
};

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

class StandaloneServiceLocator : public SEGS::IServiceLocator {
public:
    SEGS::ICompressionService *getCompression() override { return &compr; }
    SEGS::IFilesystem         *getFS() override { return &fs;}
    SEGS::ILogger             *getLogger() override { return &logger; }
private:
    QFSWrapper fs;
    QCompressor compr;
    LoggerWrapper logger;
};

void registerEnvSingleton() {
    static StandaloneServiceLocator locator;
    // setup services
    SEGS::setServiceLocator(&locator);
}
