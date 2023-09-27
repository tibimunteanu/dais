#pragma once

#include "base/base.h"
#include "core/arena.h"
#include "math/math_types.h"
#include "platform/platform_types.h"

typedef enum AppStage {
    APP_STAGE_NONE = 0,
    APP_STAGE_AWAKING = 1,
    APP_STAGE_AWAKEN = 2,
    APP_STAGE_STARTING = 3,
    APP_STAGE_STARTED = 4,
    APP_STAGE_RUNNING = 5,
    APP_STAGE_SHUTTING_DOWN = 6
} AppStage;

typedef struct AppConfig {
    char name[256];
    Vec4U32 startRect;
} AppConfig;

typedef struct App {
    AppStage stage;
    AppConfig config;

    B8 (*awake)(struct App* pApp);
    B8 (*start)(struct App* pApp);
    B8 (*update)(struct App* pApp);
    B8 (*render)(struct App* pApp);
    B8 (*shutdown)(struct App* pApp);

    void* pState;
} App;

typedef struct Engine {
    App* pApp;
    B8 isRunning;
    Arena* pArena;
    Platform platform;
} Engine;

global Engine dais;
