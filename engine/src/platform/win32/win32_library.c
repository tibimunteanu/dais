#include "base/base.h"

#ifdef OS_WINDOWS
#    include "platform/win32/win32_base.h"
#    include "platform/library.h"

void* libraryOpen(CStringLit path) {
    HMODULE module = LoadLibraryA(path);
    return (void*)module;
}

void libraryClose(void* pHandle) {
    HMODULE module = (HMODULE)pHandle;
    FreeLibrary(module);
}

void* libraryLoadFunction(void* pHandle, CStringLit name) {
    HMODULE module = (HMODULE)pHandle;
    void* pfnResult = GetProcAddress(module, name);
    return pfnResult;
}
#endif