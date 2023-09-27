#include "memory.h"

#include "core/log.h"

Arena* arenaCreate(U64 size) {
    size = roundUpToMultipleOf(size, ARENA_RESERVE_GRANULARITY);
    void* pMemory = memoryReserve(size);
    U64 commitSize = ARENA_COMMIT_GRANULARITY;

    assert(commitSize > sizeof(Arena));

    memoryCommit(pMemory, commitSize);

    Arena* pArena = (Arena*)pMemory;
    pArena->pos = sizeof(Arena);
    pArena->commitPos = commitSize;
    pArena->size = size;

    return pArena;
}

void arenaDestroy(Arena* pArena) {
    memoryRelease(pArena, pArena->size);
}

void* arenaPush(Arena* pArena, U64 size) {
    return arenaPushAligned(pArena, size, 1);
}

void* arenaPushZero(Arena* pArena, U64 size) {
    return arenaPushAlignedZero(pArena, size, 1);
}

void* arenaPushAligned(Arena* pArena, U64 size, U64 align) {
    void* pMemory = 0;

    U64 posAligned = align == 1 ? pArena->pos : alignUpPow2(pArena->pos, align);

    if (posAligned + size <= pArena->size) {
        pArena->pos = posAligned + size;
        pMemory = (U8*)pArena + posAligned;

        if (pArena->commitPos < pArena->pos) {
            U64 commitSize = pArena->pos - pArena->commitPos;
            commitSize = roundUpToMultipleOf(commitSize, ARENA_COMMIT_GRANULARITY);
            memoryCommit((U8*)pArena + pArena->commitPos, commitSize);
            pArena->commitPos += commitSize;
        }
    } else {
        // TODO: fallback to os allocation
        logError("Arena overflow");
    }
    return pMemory;
}

void* arenaPushAlignedZero(Arena* pArena, U64 size, U64 align) {
    void* pMemory = arenaPushAligned(pArena, size, align);
    if (pMemory) {
        memoryZero(pMemory, size);
    }
    return pMemory;
}

void arenaClear(Arena* pArena) {
    arenaPopTo(pArena, sizeof(Arena));
}

void arenaPop(Arena* pArena, U64 size) {
    arenaPopTo(pArena, pArena->pos - size);
}

void arenaPopTo(Arena* pArena, U64 pos) {
    pArena->pos = clamp(sizeof(Arena), pos, pArena->size);

    U64 posAlignedToCommitBoundary = roundUpToMultipleOf(pArena->pos, ARENA_COMMIT_GRANULARITY);
    if (posAlignedToCommitBoundary + ARENA_DECOMMIT_THRESHOLD < pArena->commitPos) {
        U64 decommitSize = pArena->commitPos - posAlignedToCommitBoundary;
        memoryDecommit((U8*)pArena + posAlignedToCommitBoundary, decommitSize);
        pArena->commitPos -= decommitSize;
    }
}

TempArena arenaTempBegin(Arena* pArena) {
    TempArena temp = {0};
    temp.pArena = pArena;
    temp.pos = pArena->pos;
    return temp;
}

void arenaTempEnd(TempArena tempArena) {
    arenaPopTo(tempArena.pArena, tempArena.pos);
}
