#pragma once

#include "base/base.h"
#include "core/memory_types.h"
#include "math/math_types.h"

typedef struct Window {
    void* pInternal;
} Window;

// typedef struct GammaRamp {
//     U16* pRed;
//     U16* pGreen;
//     U16* pBlue;
//     U32 size;
// } GammaRamp;

// typedef struct VideoMode {
//     I32 width;
//     I32 height;
//     I32 redBits;
//     I32 greenBits;
//     I32 blueBits;
//     I32 refreshRate;
// } VideoMode;

typedef struct Monitor {
    char name[128];
    // void* userPointer;
    // I32 widthMillimeters;
    // I32 heightMillimeters;
    // Window* pWindow;
    // VideoMode* pVideoModes;
    // I32 modeCount;
    // VideoMode currentVideoMode;
    // GammaRamp originalGammaRamp;
    // GammaRamp currentGammaRamp;
    void* pInternal;
} Monitor;

typedef void (*PFN_monitorCallback)(Monitor* pMonitor, I32 event);

typedef struct Platform {
    Monitor** pMonitors;
    U32 monitorCount;

    void* pInternal;
} Platform;

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

typedef struct Dais {
    App* pApp;
    B8 isRunning;
    Arena* pArena;
    Platform platform;
} Dais;

global Dais dais;
