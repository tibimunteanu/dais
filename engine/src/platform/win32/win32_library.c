#include "base/base.h"

#if defined(OS_WINDOWS)

    #include "platform/win32/win32_base.h"
    #include "platform/library.h"

pub void* libraryOpen(const CString path) {
    HMODULE module = LoadLibraryA(path);
    return (void*)module;
}

pub void libraryClose(void* pHandle) {
    HMODULE module = (HMODULE)pHandle;
    FreeLibrary(module);
}

pub void* libraryLoadFunction(void* pHandle, const CString name) {
    HMODULE module = (HMODULE)pHandle;
    void* pfnResult = GetProcAddress(module, name);
    return pfnResult;
}

#endif /* if defined(OS_WINDOWS) */
