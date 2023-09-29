#pragma once

#include "base/base.h"

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


#if !defined(ARENA_RESERVE_GRANULARITY)
    #define ARENA_RESERVE_GRANULARITY megabytes(64)
#endif

#if !defined(ARENA_COMMIT_GRANULARITY)
    #define ARENA_COMMIT_GRANULARITY kilobytes(4)
#endif

#if !defined(ARENA_DECOMMIT_THRESHOLD)
    #define ARENA_DECOMMIT_THRESHOLD megabytes(64)
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


#define arenaPushStruct(pArena, T)                   (T*)arenaPush(pArena, sizeof(T))
#define arenaPushStructZero(pArena, T)               (T*)arenaPushZero(pArena, sizeof(T))
#define arenaPushStructAligned(pArena, T, align)     (T*)arenaPushAligned(pArena, sizeof(T), (align))
#define arenaPushStructAlignedZero(pArena, T, align) (T*)arenaPushAlignedZero(pArena, sizeof(T), (align))

#define arenaPushArray(pArena, T, count)               (T*)arenaPush(pArena, sizeof(T) * (count))
#define arenaPushArrayZero(pArena, T, count)           (T*)arenaPushZero(pArena, sizeof(T) * (count))
#define arenaPushArrayAligned(pArena, T, count, align) (T*)arenaPushAligned(pArena, sizeof(T) * (count), (align))
#define arenaPushArrayAlignedZero(pArena, T, count, align) (T*)arenaPushAlignedZero( \
    pArena,                                                                          \
    sizeof(T) * (count),                                                             \
    (align)                                                                          \
)


API TempArena arenaTempBegin(Arena* pArena);
API void arenaTempEnd(TempArena tempArena);

#define arenaTempBlock(pArena)                                  \
        TempArena IDENTIFIER_FROM_LINE(_tempArena_) = { 0 };    \
        DEFER_BLOCK(                                            \
    IDENTIFIER_FROM_LINE(_tempArena_) = arenaTempBegin(pArena), \
    arenaTempEnd(IDENTIFIER_FROM_LINE(_tempArena_))             \
        )
