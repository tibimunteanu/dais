#include "base/base.h"

#if defined(OS_WINDOWS)

    #include "platform/win32/win32_base.h"
    #include "platform/win32/win32_platform_types.h"
    #include "platform/window.h"
    #include "core/log.h"
    #include "core/arena.h"
    #include "math/math_types.h"

pub fn windowCreate(Arena* pArena, const CString title, Vec4U32 rect, Window* out_pWindow) {
    Win32Platform* win32Platform = pDais->pPlatform->pInternal;

    memoryZero(out_pWindow, sizeof(Window));

    HICON icon = LoadIcon(win32Platform->instance, IDI_APPLICATION);

    WNDCLASSA windowClass = {
        .style = CS_DBLCLKS,
        .lpfnWndProc = DefWindowProc,
        .hInstance = win32Platform->instance,
        .hIcon = icon,
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .lpszClassName = "DaisWindowClass"
    };

    if (!RegisterClassA(&windowClass)) {
        error("Failed to register window class");
    }

    Vec4U32 windowRect = rect;
    DWORD windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME;
    DWORD windowStyleEx = WS_EX_APPWINDOW;
    RECT borderRect = {0, 0, 0, 0};

    AdjustWindowRectEx(&borderRect, windowStyle, FALSE, windowStyleEx);

    windowRect.x += borderRect.left;
    windowRect.y += borderRect.top;
    windowRect.z += borderRect.right - borderRect.left;
    windowRect.w += borderRect.bottom - borderRect.top;

    HWND windowHandle = CreateWindowExA(
        windowStyleEx,
        windowClass.lpszClassName,
        title,
        windowStyle,
        windowRect.x,
        windowRect.y,
        windowRect.z,
        windowRect.w,
        0,
        0,
        win32Platform->instance,
        0
    );

    if (!windowHandle) {
        error("Failed to create window");
    }

    Win32Window* win32Window = arenaPushStructZero(pArena, Win32Window);

    win32Window->handle = windowHandle;

    out_pWindow->pInternal = win32Window;

    ShowWindow(windowHandle, SW_SHOW);

    ok();
}

#endif /* if defined(OS_WINDOWS) */
