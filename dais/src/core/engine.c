#include "core/engine.h"
#include "core/log.h"
#include "renderer/vulkan/vulkan_renderer.h"

Engine engine = {0};

B8 engineAwake(void) {
    engine.pApp = NULL;
    engine.isRunning = false;
    engine.pArena = arenaCreate(gigabytes(1));

    // Log init
    LogConfig logConfig = {
        .fileName = "log.txt",
    };
    if (!logInit(engine.pArena, logConfig)) {
        return false;
    }

    return true;
}

B8 engineStart(App* pApp) {
    engine.pApp = pApp;

    pApp->stage = APP_STAGE_NONE;

    Window window = {0};
    if (!osWindowCreate(engine.pArena, engine.pApp->config.name, engine.pApp->config.startRect, &window)) {
        logFatal("Failed to create window");
        return false;
    }

    if (!vulkanRendererInit(&window)) {
        logFatal("Failed to initialize vulkan renderer");
    }

    pApp->stage = APP_STAGE_AWAKING;
    if (!pApp->awake(pApp)) {
        logFatal("Failed to awake app");
        return false;
    }
    pApp->stage = APP_STAGE_AWAKEN;

    pApp->stage = APP_STAGE_STARTING;
    if (!pApp->start(pApp)) {
        logFatal("Failed to start app");
        return false;
    }
    pApp->stage = APP_STAGE_STARTED;

    pApp->stage = APP_STAGE_RUNNING;
    engine.isRunning = true;

    // while (engine.isRunning) {
    //     // TODO: app loop
    // }

    return true;
}

B8 engineShutdown(App* pApp) {
    B8 shutdownOk = true;

    engine.isRunning = false;

    pApp->stage = APP_STAGE_SHUTTING_DOWN;
    if (!pApp->shutdown(pApp)) {
        logError("Failed to shutdown app");
        shutdownOk = false;
    }
    pApp->stage = APP_STAGE_NONE;

    vulkanRendererRelease();
    logRelease();
    arenaDestroy(engine.pArena);

    memoryZeroStruct(&engine);

    return shutdownOk;
}
