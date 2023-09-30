#include "pool.h"
#include "core/log.h"

// NOTE: could we have a defragment function of type one shot computation fee.
// you could store it in the same arena if you can afford the possible wasted space or you can fix the damage done by
// another one shot computation. this would be easier if pointers were wrapped in handles. indices are very important
// and should be favored in critical paths.

// this pool is simple. it doesn't guarantee that the allocations are
// contiguous, but if you don't do more allocations into this arena while
// adding all entries, they can be. the pool is associated to an arena and
// cannot be shared with other arenas.

// this should be used in situations where you don't require that all slots be
// contiguous in memory, and you don't 100% the max number of slots your program
// will need at runtime. you can ballpark a reservedSlots count, but allow it to
// grow as needed. it's different than just pushing to the arena because you can
// free any slot and reuse it later.

// NOTE: the pool cannot be cleared because we don't keep track of all the
// allocated pointers. only call sites can decide to hold on to a pointer and
// free it later.
Pool* poolCreate(Arena* pArena, PoolCreateInfo poolCreateInfo) {
    U64 slotSize = poolCreateInfo.slotSize;
    U64 slotAlignment = poolCreateInfo.slotAlignment;

    if (slotAlignment > 1) {
        slotSize = alignUpPow2(slotSize, slotAlignment);
    }

    assertMsg(slotSize >= sizeof(PoolFreeNode), "Pool slot size is too small");

    Pool* pPool = arenaPushStruct(pArena, Pool);
    pPool->slotSize = slotSize;
    pPool->slotAlignment = slotAlignment;
    pPool->pArena = pArena;
    pPool->pHead = NULL;

    U64 reservedSlots = poolCreateInfo.reservedSlots;
    U8* pSlots = arenaPushAlignedZero(pArena, reservedSlots * slotSize, slotAlignment);

    for (I64 i = reservedSlots - 1; i >= 0; i--) {
        PoolFreeNode* pFreeNode = (PoolFreeNode*)(pSlots + (i * slotSize));

        pFreeNode->next = pPool->pHead;
        pPool->pHead = pFreeNode;
    }

    return pPool;
}

void* poolAlloc(Pool* pPool) {
    if (!pPool->pHead) {
        pPool->pHead = arenaPushAlignedZero(pPool->pArena, pPool->slotSize, pPool->slotAlignment);
    }

    PoolFreeNode* pResult = pPool->pHead;
    pPool->pHead = pPool->pHead->next;
    pResult->next = NULL; // allocating it clears the first pointer back to zero

    return pResult;
}

void poolFree(Pool* pPool, void* pPtr) {
    // this function is also like a brute allocator. it doesn't check if the
    // pPtr is actually a slot in the pool. I'd rather pay in dev time due
    // diligence that in runtime bloat. I am just not going to call it with
    // something other than a pointer in the pool (this again would be a non
    // issue if handles were free, but maybe think about if it's not that much
    // overhead in wrapping all pointers in a codebase by handles. does it turn
    // into a handle alloc pb?)
    // we only zero it so that if we are somehow given memory outside the
    // bounds of the pool or from outside the pool, the effects of erasing it
    // should be instant so it's easier to track down rather than it happening at a later time
    memoryZero(pPtr, pPool->slotSize);

    PoolFreeNode* pPtrFreeNode = (PoolFreeNode*)pPtr;
    pPtrFreeNode->next = pPool->pHead;
    pPool->pHead = pPtrFreeNode;
}
