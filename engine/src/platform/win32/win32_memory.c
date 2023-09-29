#include "base/base.h"

#if defined(OS_WINDOWS)

    #include "platform/win32/win32_base.h"
    #include "platform/win32/win32_platform_types.h"
    #include "platform/memory.h"

public U64 memoryPageSizeGet(void) {
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwPageSize;
}

public void* memoryReserve(U64 size) {
    size = roundUpToMultipleOf(size, gigabytes(1));
    void* pResult = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
    return pResult;
}

public void memoryRelease(void* pMemory, U64 size) {
    VirtualFree(pMemory, 0, MEM_RELEASE);
}

public void memoryCommit(void* pMemory, U64 size) {
    size = roundUpToMultipleOf(size, memoryPageSizeGet());
    VirtualAlloc(pMemory, size, MEM_COMMIT, PAGE_READWRITE);
}

public void memoryDecommit(void* pMemory, U64 size) {
    VirtualFree(pMemory, size, MEM_DECOMMIT);
}

public void memoryProtect(void* pMemory, U64 size, MemoryAccessFlags flags) {
    size = roundUpToMultipleOf(size, memoryPageSizeGet());

    DWORD newFlags = 0;
    switch (flags) {
        case MEMORY_ACCESS_FLAGS_READ: newFlags = PAGE_READONLY; break;
        case MEMORY_ACCESS_FLAGS_READ | MEMORY_ACCESS_FLAGS_WRITE: newFlags = PAGE_READWRITE; break;
        case MEMORY_ACCESS_FLAGS_EXECUTE: newFlags = PAGE_EXECUTE; break;
        case MEMORY_ACCESS_FLAGS_EXECUTE | MEMORY_ACCESS_FLAGS_READ: newFlags = PAGE_EXECUTE_READ; break;
        case MEMORY_ACCESS_FLAGS_EXECUTE | MEMORY_ACCESS_FLAGS_READ |
            MEMORY_ACCESS_FLAGS_WRITE: newFlags = PAGE_EXECUTE_READWRITE; break;
        case MEMORY_ACCESS_FLAGS_EXECUTE | MEMORY_ACCESS_FLAGS_WRITE: newFlags = PAGE_EXECUTE_WRITECOPY; break;
        default: newFlags = PAGE_NOACCESS; break;
    }

    DWORD oldFlags = 0;
    VirtualProtect(pMemory, size, newFlags, &oldFlags);
}

#endif /* if defined(OS_WINDOWS) */
