#include "engine.h"

#include "core/log.h"
#include "core/arena.h"
#include "renderer/vulkan/vulkan_renderer.h"

pub Engine* pDais = NULL;

//
prv Result _loadGameLibrary(void) {
    void* pGameLib = libraryOpen("game.dll");

    if (!pGameLib) {
        panic("Failed to open game library");
    }

    pDais->pGame->configure = libraryLoadFunction(pGameLib, "configure");
    pDais->pGame->awake = libraryLoadFunction(pGameLib, "awake");
    pDais->pGame->start = libraryLoadFunction(pGameLib, "start");
    pDais->pGame->update = libraryLoadFunction(pGameLib, "update");
    pDais->pGame->render = libraryLoadFunction(pGameLib, "render");
    pDais->pGame->shutdown = libraryLoadFunction(pGameLib, "shutdown");

    if (!pDais->pGame->configure || !pDais->pGame->awake || !pDais->pGame->start || !pDais->pGame->update ||
        !pDais->pGame->render || !pDais->pGame->shutdown) {
        panic("Failed to load game function pointers");
    }

    return OK;
}

//
pub Result engineRun(void) {
    Arena* pArena = arenaCreate(gigabytes(1));

    pDais = arenaPushStructZero(pArena, Engine);

    // Log init
    LogConfig logConfig = {
        .fileName = "log.txt",
    };

    try(logInit(pArena, logConfig));

    pDais->pGame = arenaPushStructZero(pArena, Game);

    try(_loadGameLibrary());

    pDais->pGame->config = pDais->pGame->configure();
    pDais->pGame->stage = GAME_STAGE_NONE;

    try(platformInit(pArena));

    Window window = {0};
    try(windowCreate(pArena, pDais->pGame->config.name, pDais->pGame->config.startRect, &window));

    try(vulkanRendererInit(pArena, pDais->pPlatform, &window));

    pDais->pGame->stage = GAME_STAGE_AWAKING;
    try(pDais->pGame->awake());
    pDais->pGame->stage = GAME_STAGE_AWAKEN;

    pDais->pGame->stage = GAME_STAGE_STARTING;
    try(pDais->pGame->start());
    pDais->pGame->stage = GAME_STAGE_STARTED;

    // loop
    pDais->pGame->stage = GAME_STAGE_RUNNING;
    pDais->isRunning = true;

    // while (engine.isRunning) {
    //     // TODO: game loop
    // }

    // shutdown
    pDais->isRunning = false;

    pDais->pGame->stage = GAME_STAGE_SHUTTING_DOWN;
    alert(pDais->pGame->shutdown());
    pDais->pGame->stage = GAME_STAGE_NONE;

    vulkanRendererRelease();
    logRelease();

    alert(platformRelease());

    arenaDestroy(pArena);
    pArena = NULL;

    return OK;
}
