#pragma once

#include "base/base.h"

#ifndef NOMINMAX
#    define NOMINMAX
#endif

#ifndef VC_EXTRALEAN
#    define VC_EXTRALEAN
#endif

#ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#endif

#ifndef UNICODE
#    define UNICODE
#endif

#include <windows.h>
#include <dbt.h>

typedef struct Win32Monitor {
    HMONITOR handle;
    WCHAR adapterName[32];
    WCHAR displayName[32];
    char publicAdapterName[32];
    char publicDisplayName[32];
    // B32 modesPruned;
    // B32 modeChanged;
} Win32Monitor;

typedef struct Win32Window {
    HWND handle;
} Win32Window;

typedef struct Win32Platform {
    HINSTANCE instance;
    HWND helperWindowHandle;
    ATOM helperWindowClass;
    HWND mainWindowClass;
    HDEVNOTIFY deviceNotificationHandle;

} Win32Platform;
