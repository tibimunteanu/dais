#include "dais.h"

#include "core/log.h"
#include "core/memory.h"
#include "renderer/vulkan/vulkan_renderer.h"

Dais dais = {0};

B8 daisAwake(void) {
    dais.pArena = arenaCreate(gigabytes(1));

    // Log init
    LogConfig logConfig = {
        .fileName = "log.txt",
    };
    if (!logInit(dais.pArena, logConfig)) {
        return false;
    }

    return true;
}

B8 daisStart(App* pApp) {
    dais.pApp = pApp;

    pApp->stage = APP_STAGE_NONE;

    if (!platformInit()) {
        logFatal("Failed to initialize the platform");
        return false;
    }

    Window window = {0};
    if (!windowCreate(dais.pArena, dais.pApp->config.name, dais.pApp->config.startRect, &window)) {
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
    dais.isRunning = true;

    // while (engine.isRunning) {
    //     // TODO: app loop
    // }

    return true;
}

B8 daisShutdown(App* pApp) {
    B8 shutdownOk = true;

    dais.isRunning = false;

    pApp->stage = APP_STAGE_SHUTTING_DOWN;
    if (!pApp->shutdown(pApp)) {
        logError("Failed to shutdown app");
        shutdownOk = false;
    }
    pApp->stage = APP_STAGE_NONE;

    vulkanRendererRelease();
    logRelease();
    platformRelease();
    arenaDestroy(dais.pArena);

    memoryZeroStruct(&dais);

    return shutdownOk;
}
