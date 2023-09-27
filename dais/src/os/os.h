#pragma once

#include "base/base.h"
#include "core/arena.h"
#include "math/math_types.h"

typedef U32 MemoryAccessFlags;
enum {
    MEMORY_ACCESS_FLAGS_READ = 1 << 0,
    MEMORY_ACCESS_FLAGS_WRITE = 1 << 1,
    MEMORY_ACCESS_FLAGS_EXECUTE = 1 << 2,
    MEMORY_ACCESS_FLAGS_CREATE_NEW = 1 << 3,
    MEMORY_ACCESS_FLAGS_SHARED = 1 << 4,
};

typedef struct Window {
    void* pOsState;
} Window;

typedef struct GammaRamp {
    U16* pRed;
    U16* pGreen;
    U16* pBlue;
    U32 size;
} GammaRamp;

typedef struct VideoMode {
    I32 width;
    I32 height;
    I32 redBits;
    I32 greenBits;
    I32 blueBits;
    I32 refreshRate;
} VideoMode;

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
    void* pOsState;
} Monitor;

typedef void (*PFN_monitorCallback)(Monitor* pMonitor, I32 event);

// memory
API U64 osMemoryPageSizeGet(void);
API void* osMemoryReserve(U64 size);
API void osMemoryRelease(void* pMemory, U64 size);
API void osMemoryCommit(void* pMemory, U64 size);
API void osMemoryDecommit(void* pMemory, U64 size);
API void osMemoryProtect(void* pMemory, U64 size, MemoryAccessFlags flags);

// library
API void* osLibraryOpen(const char* path);
API void osLibraryClose(void* pHandle);
API void* osLibraryLoadFunction(void* pHandle, const char* name);

// monitor
// API Monitor** osMonitorsGetAll();
// API Monitor* osMonitorGetPrimary();
// API void osMonitorGetPosition(Monitor* pMonitor, I32* out_pPosX, I32* out_pPosY);
// API void osMonitorGetWorkarea(Monitor* pMonitor, I32* out_pPosX, I32* out_pPosY, I32* out_pWidth, I32* out_pHeight);
// API void osMonitorGetPhysicalSize(Monitor* pMonitor, I32* out_pWidthMillimeters, I32* out_pHeightMillimeters);
// API void osMonitorGetContentScale(Monitor* pMonitor, F32* out_pScaleX, F32* out_pScaleY);
// API const char* osMonitorGetName(Monitor* pMonitor);
// API void osMonitorSetUserPointer(Monitor* pMonitor, void* pUserData);
// API void* osMonitorGetUserPointer(Monitor* pMonitor);
// API const VideoMode* osMonitorGetVideoModes(Monitor* pMonitor, I32* out_pCount);
// API const VideoMode* osMonitorGetVideoMode(Monitor* pMonitor);
// API void osMonitorSetGamma(Monitor* pMonitor, F32 gamma);
// API const GammaRamp* osMonitorGetGammaRamp(Monitor* pMonitor);
// API void osMonitorSetGammaRamp(Monitor* pMonitor, const GammaRamp* pGammaRamp);
// API PFN_monitorCallback osMonitorSetCallback(PFN_monitorCallback pfnCallback);

// window
API B8 osWindowCreate(Arena* pArena, const char* title, Vec4U32 rect, Window* out_pWindow);
