#include "game.h"

internal Arena* pArena;

GameConfig configure(void) {
    return (GameConfig) {
        .name = "Sandbox",
        .startRect = (Vec4U32) {200, 200, 960, 540},
    };
}

Result awake(void) {
    logInfo("Awakening game");

    pArena = arenaCreate(gigabytes(1));

    arenaTempBlock(pArena) {
        logInfo("Begin arena temp with reserved %llu bytes and pos %llu", pArena->size, pArena->pos);
        arenaPush(pArena, 100);
        arenaPushArrayAligned(pArena, U8, 100, 8);
        arenaPushZero(pArena, 100);
        logInfo("After push pos %llu", pArena->pos);
    }
    logInfo("After temp block pos is %llu", pArena->pos);

    return OK;
}

Result start(void) {
    logInfo("Starting game");

    // panic("Panic at START");

    U64 stall = 10000000000;
    while (stall--)
        ;

    return OK;
}

Result update(void) {
    return OK;
}

Result render(void) {
    return OK;
}

Result shutdown(void) {
    logInfo("Shutting down game");

    // panic("If shutdown panics, the engine still continues shutting down");

    arenaDestroy(pArena);
    pArena = NULL;

    return OK;
}
