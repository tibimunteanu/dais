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

typedef struct Arena {
    U64 pos;
    U64 commitPos;
    U64 size;
    U64 __padding__;
} Arena;

typedef struct TempArena {
    Arena* pArena;
    U64 pos;
} TempArena;
