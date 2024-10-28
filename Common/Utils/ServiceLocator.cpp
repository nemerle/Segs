#include "IServiceLocator.h"

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
}