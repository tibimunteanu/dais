#include "game.h"

prv Arena* pArena;

//
pub GameConfig configure(void) {
    return (GameConfig) {
        .name = "Sandbox",
        .startRect = (Vec4U32) {200, 200, 960, 540},
    };
}

pub Result awake(void) {
    logInfo("Awakening game");

    pArena = arenaCreate(gigabytes(1));

    arenaTempBlock(pArena) {
        logInfo("Begin arena temp with reserved %llu bytes and pos %llu", pArena->size, pArena->pos);
        arenaPush(pArena, 100);
        arenaPushArrayAligned(pArena, U8, 100, 8);
        arenaPushZero(pArena, 100);
        logInfo("After push pos %llu", pArena->pos);

        Pool* pPool = poolCreate(
            pArena,
            (PoolCreateInfo) {
                .slotSize = 16,
                .slotAlignment = 32,
                .reservedSlots = 10,
            }
        );

        logInfo("After pool creation %llu", pArena->pos);

        (void)poolAlloc(pPool);

        logInfo("After 1 chunk alloc %llu", pArena->pos);

        void* block = NULL;
        for (I32 i = 0; i < 9; i++) {
            block = poolAlloc(pPool);
        }

        logInfo("After 9 more chunk alloc %llu", pArena->pos);

        poolFree(pPool, block);

        logInfo("After 1 chunk free %llu", pArena->pos);

        for (I32 i = 0; i < 11; i++) {
            block = poolAlloc(pPool);
        }

        logInfo("After 10 more chunk alloc %llu", pArena->pos);
    }
    logInfo("After temp block pos is %llu", pArena->pos);

    return OK;
}

pub Result start(void) {
    logInfo("Starting game");

    panic("Panic at START");

    U64 stall = 10000000000;
    while (stall--) {
    }

    return OK;
}

pub Result update(void) {
    return OK;
}

pub Result render(void) {
    return OK;
}

pub Result shutdown(void) {
    logInfo("Shutting down game");

    arenaDestroy(pArena);
    pArena = NULL;

    return OK;
}
