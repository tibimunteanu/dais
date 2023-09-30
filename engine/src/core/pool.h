#pragma once

#include "base/base.h"
#include "core/arena.h"

typedef struct PoolFreeNode {
    struct PoolFreeNode* next;
} PoolFreeNode;

typedef struct PoolCreateInfo {
    U64 slotSize;
    U64 slotAlignment;
    U64 reservedSlots;
} PoolCreateInfo;

typedef struct Pool {
    U64 slotSize;
    U64 slotAlignment;
    Arena* pArena;
    PoolFreeNode* pHead;
} Pool;

API Pool* poolCreate(Arena* pArena, PoolCreateInfo poolCreateInfo);
API void* poolAlloc(Pool* pPool);
API void poolFree(Pool* pPool, void* pPtr);
