#include "FilesystemHandler.h"
#include "IServiceLocator.h"
#include "Utils/PiggFsHandler.h"

namespace SEGS
{
    IServiceLocator *g_service_locator = nullptr;
    void setServiceLocator(IServiceLocator *sl)
    {
        g_service_locator = sl;
    }
    IServiceLocator *getServiceLocator()
    {
        return g_service_locator;
    }

    BaseServiceLocator::BaseServiceLocator(FilesystemFactory &&native_fs)
    {
        m_fs = eastl::make_unique<RootFilesystem>();
        m_fs->registerFactory("",native_fs);
        m_fs->registerFactory("pigg", createPiggFilesystemFactory());;
        m_fs->mount("/","/",-99);
    }

    BaseServiceLocator::~BaseServiceLocator() {

    }
}
