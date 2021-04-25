#include "Platform.h"

namespace dais
{
    void* ThreadLocalStorage::Get()
    {
        return PlatformGet();
    }

    void ThreadLocalStorage::Set(void* value)
    {
        PlatformSet(value);
    }
}
