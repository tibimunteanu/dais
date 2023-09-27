#include "game.h"

internal Arena* pArena;

B8 appRegister(App* pApp) {
    pApp->config = (AppConfig) {
        .name = "Sandbox",
        .startRect = (Vec4U32) {200, 200, 960, 540},
    };

    pApp->awake = awake;
    pApp->start = start;
    pApp->update = update;
    pApp->render = render;
    pApp->shutdown = shutdown;

    pApp->pState = NULL;

    return true;
}

B8 awake(App* pApp) {
    logInfo("Awakening app");

    pArena = arenaCreate(gigabytes(1));

    arenaTempBlock(pArena) {
        logInfo("Begin arena temp with reserved %llu bytes and pos %llu", pArena->size, pArena->pos);
        arenaPush(pArena, 100);
        arenaPushArrayAligned(pArena, U8, 100, 8);
        arenaPushZero(pArena, 100);
        logInfo("After push pos %llu", pArena->pos);
    }
    logInfo("After temp block pos is %llu", pArena->pos);

    return true;
}

B8 start(App* pApp) {
    logInfo("Starting app");

    U64 stall = 10000000000;
    while (stall--)
        ;

    return true;
}

B8 update(App* pApp) {
    return true;
}

B8 render(App* pApp) {
    return true;
}

B8 shutdown(App* pApp) {
    logInfo("Shutting down app");

    arenaDestroy(pArena);
    pArena = NULL;

    return true;
}
