#pragma once

#include "base/base.h"

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
