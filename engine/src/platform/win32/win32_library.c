#include "base/base.h"

#ifdef OS_WINDOWS
    #include "platform/win32/win32_base.h"
    #include "platform/library.h"

public void* libraryOpen(CStringLit path) {
    HMODULE module = LoadLibraryA(path);
    return (void*)module;
}

public void libraryClose(void* pHandle) {
    HMODULE module = (HMODULE)pHandle;
    FreeLibrary(module);
}

public void* libraryLoadFunction(void* pHandle, CStringLit name) {
    HMODULE module = (HMODULE)pHandle;
    void* pfnResult = GetProcAddress(module, name);
    return pfnResult;
}

#endif
