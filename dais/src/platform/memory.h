#pragma once

#include "base/base.h"

typedef U32 MemoryAccessFlags;
enum {
    MEMORY_ACCESS_FLAGS_READ = 1 << 0,
    MEMORY_ACCESS_FLAGS_WRITE = 1 << 1,
    MEMORY_ACCESS_FLAGS_EXECUTE = 1 << 2,
    MEMORY_ACCESS_FLAGS_CREATE_NEW = 1 << 3,
    MEMORY_ACCESS_FLAGS_SHARED = 1 << 4,
};

API U64 memoryPageSizeGet(void);
API void* memoryReserve(U64 size);
API void memoryRelease(void* pMemory, U64 size);
API void memoryCommit(void* pMemory, U64 size);
API void memoryDecommit(void* pMemory, U64 size);
API void memoryProtect(void* pMemory, U64 size, MemoryAccessFlags flags);
