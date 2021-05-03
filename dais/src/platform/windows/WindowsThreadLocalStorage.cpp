#include "platform/windows/WindowsPlatform.h"

namespace dais
{
    ThreadLocalStorage* ThreadLocalStorage::Create()
    {
        DWORD index = TlsAlloc();
        if (index == TLS_OUT_OF_INDEXES)
        {
            DAIS_ERROR_WIN32("Failed to allocate TLS index!");
            return nullptr;
        }

        WindowsThreadLocalStorage* tls = new WindowsThreadLocalStorage();
        tls->m_Index = index;
        tls->m_Allocated = true;

        return tls;
    }

    WindowsThreadLocalStorage::~WindowsThreadLocalStorage()
    {
        if (m_Allocated)
        {
            TlsFree(m_Index);
        }
    }

    void* WindowsThreadLocalStorage::PlatformGet()
    {
        if (m_Allocated)
        {
            return TlsGetValue(m_Index);
        }
        return nullptr;
    }

    void WindowsThreadLocalStorage::PlatformSet(void* value)
    {
        if (m_Allocated)
        {
            TlsSetValue(m_Index, value);
        }
    }
}
