#pragma once

#include "base/base.h"
#include "core/memory_types.h"

API U64 memoryPageSizeGet(void);
API void* memoryReserve(U64 size);
API void memoryRelease(void* pMemory, U64 size);
API void memoryCommit(void* pMemory, U64 size);
API void memoryDecommit(void* pMemory, U64 size);
API void memoryProtect(void* pMemory, U64 size, MemoryAccessFlags flags);

// arena
#if !defined(ARENA_RESERVE_GRANULARITY)
#    define ARENA_RESERVE_GRANULARITY megabytes(64)
#endif

#if !defined(ARENA_COMMIT_GRANULARITY)
#    define ARENA_COMMIT_GRANULARITY kilobytes(4)
#endif

#if !defined(ARENA_DECOMMIT_THRESHOLD)
#    define ARENA_DECOMMIT_THRESHOLD megabytes(64)
#endif

API Arena* arenaCreate(U64 size);
API void arenaDestroy(Arena* pArena);
API void* arenaPush(Arena* pArena, U64 size);
API void* arenaPushZero(Arena* pArena, U64 size);
API void* arenaPushAligned(Arena* pArena, U64 size, U64 align);
API void* arenaPushAlignedZero(Arena* pArena, U64 size, U64 align);
API void arenaClear(Arena* pArena);
API void arenaPop(Arena* pArena, U64 size);
API void arenaPopTo(Arena* pArena, U64 pos);

#define arenaPushArray(pArena, T, count)     (T*)arenaPush(pArena, sizeof(T) * (count))
#define arenaPushArrayZero(pArena, T, count) (T*)arenaPushZero(pArena, sizeof(T) * (count))

#define arenaPushArrayAligned(pArena, T, count, align) (T*)arenaPushAligned(pArena, sizeof(T) * (count), (align))

#define arenaPushArrayAlignedZero(pArena, T, count, align) \
    (T*)arenaPushAlignedZero(pArena, sizeof(T) * (count), (align))

// arenaTemp
API TempArena arenaTempBegin(Arena* pArena);
API void arenaTempEnd(TempArena tempArena);

#define arenaTempBlock(pArena)                                                                                      \
    TempArena IDENTIFIER_FROM_LINE(_tempArena_) = {0};                                                              \
    DEFER_BLOCK(                                                                                                    \
        IDENTIFIER_FROM_LINE(_tempArena_) = arenaTempBegin(pArena), arenaTempEnd(IDENTIFIER_FROM_LINE(_tempArena_)) \
    )