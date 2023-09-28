#include "engine.h"

#include "core/log.h"
#include "core/arena.h"
#include "renderer/vulkan/vulkan_renderer.h"

Engine* pDais = NULL;

B8 engineRun(void) {
    Arena* pArena = arenaCreate(gigabytes(1));

    pDais = arenaPushZero(pArena, sizeof(Engine));

    // Log init
    LogConfig logConfig = {
        .fileName = "log.txt",
    };
    if (!logInit(pArena, logConfig)) {
        return false;
    }

    pDais->pGame = arenaPushZero(pArena, sizeof(Game));

    void* pSandboxLib = libraryOpen("sandbox.dll");

    pDais->pGame->configure = libraryLoadFunction(pSandboxLib, "configure");
    pDais->pGame->awake = libraryLoadFunction(pSandboxLib, "awake");
    pDais->pGame->start = libraryLoadFunction(pSandboxLib, "start");
    pDais->pGame->update = libraryLoadFunction(pSandboxLib, "update");
    pDais->pGame->render = libraryLoadFunction(pSandboxLib, "render");
    pDais->pGame->shutdown = libraryLoadFunction(pSandboxLib, "shutdown");

    if (!pDais->pGame->configure || !pDais->pGame->awake || !pDais->pGame->start || !pDais->pGame->update ||
        !pDais->pGame->render || !pDais->pGame->shutdown) {
        logFatal("Game is missing required functions");
        return false;
    }

    pDais->pGame->config = pDais->pGame->configure();
    pDais->pGame->stage = GAME_STAGE_NONE;

    if (!platformInit(pArena)) {
        logFatal("Failed to initialize the platform");
        return false;
    }

    Window window = {0};
    if (!windowCreate(pArena, pDais->pGame->config.name, pDais->pGame->config.startRect, &window)) {
        logFatal("Failed to create window");
        return false;
    }

    if (!vulkanRendererInit(pArena, pDais->pPlatform, &window)) {
        logFatal("Failed to initialize vulkan renderer");
    }

    pDais->pGame->stage = GAME_STAGE_AWAKING;
    if (!pDais->pGame->awake()) {
        logFatal("Failed to awake game");
        return false;
    }
    pDais->pGame->stage = GAME_STAGE_AWAKEN;

    pDais->pGame->stage = GAME_STAGE_STARTING;
    if (!pDais->pGame->start()) {
        logFatal("Failed to start game");
        return false;
    }
    pDais->pGame->stage = GAME_STAGE_STARTED;

    // loop
    pDais->pGame->stage = GAME_STAGE_RUNNING;
    pDais->isRunning = true;

    // while (engine.isRunning) {
    //     // TODO: game loop
    // }

    // shutdown
    B8 shutdownOk = true;

    pDais->isRunning = false;

    pDais->pGame->stage = GAME_STAGE_SHUTTING_DOWN;
    if (!pDais->pGame->shutdown()) {
        logError("Failed to shutdown game");
        shutdownOk = false;
    }
    pDais->pGame->stage = GAME_STAGE_NONE;

    vulkanRendererRelease();
    logRelease();
    platformRelease();

    memoryZeroStruct(&pDais);

    arenaDestroy(pArena);
    pArena = NULL;

    return shutdownOk;
}
