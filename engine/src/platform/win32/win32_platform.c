#include "base/base.h"

#ifdef OS_WINDOWS
    #include "platform/win32/win32_base.h"
    #include "platform/win32/win32_platform_types.h"
    #include "core/log.h"
    #include "core/arena.h"

private void _pollMonitors(void) {
    logInfo("Polling monitors...");
}

private LRESULT CALLBACK _helperWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DISPLAYCHANGE: {
            _pollMonitors();
            break;
        }

        case WM_DEVICECHANGE: {
            // if (!joysticksInitialized) {
            //     break;
            // }

            // if (wParam == DBT_DEVICEARRIVAL) {
            //     DEV_BROADCAST_HDR* dbh = (DEV_BROADCAST_HDR*)lParam;
            //     if (dbh && dbh->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
            //         osDetectJoystickConnectionWin32();
            //     }
            // } else if (wParam == DBT_DEVICEREMOVECOMPLETE) {
            //     DEV_BROADCAST_HDR* dbh = (DEV_BROADCAST_HDR*)lParam;
            //     if (dbh && dbh->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
            //         osDetectJoystickDisconnectionWin32();
            //     }
            // }

            break;
        }
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

private Result _getModuleHandle(void) {
    Win32Platform* pWin32Platform = pDais->pPlatform->pInternal;

    if (!GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (const WCHAR*)&pDais,
        &pWin32Platform->instance
        )) {
        panic("Failed to get the module handle");
    }

    return OK;
}

private Result _createHelperWindow(void) {
    Win32Platform* pWin32Platform = pDais->pPlatform->pInternal;

    WNDCLASSEXW windowClassCreateInfo = {
        .cbSize = sizeof(WNDCLASSEXW),
        .style = CS_OWNDC,
        .lpfnWndProc = (WNDPROC)_helperWindowProc,
        .hInstance = pWin32Platform->instance,
        .lpszClassName = L"DaisHelperWindow",
    };

    pWin32Platform->helperWindowClass = RegisterClassExW(&windowClassCreateInfo);

    if (!pWin32Platform->helperWindowClass) {
        panic("Failed to register helper window class");
    }

    pWin32Platform->helperWindowHandle = CreateWindowExW(
        WS_EX_OVERLAPPEDWINDOW,
        MAKEINTATOM(pWin32Platform->helperWindowClass),
        L"DaisHelperWindow",
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0,
        0,
        1,
        1,
        NULL,
        NULL,
        pWin32Platform->instance,
        NULL
    );

    if (!pWin32Platform->helperWindowHandle) {
        panic("Failed to create helper window");
    }

    // NOTE: if a STARTUPINFO is passed along, the first ShowWindow is ignored
    ShowWindow(pWin32Platform->helperWindowHandle, SW_HIDE);

    DEV_BROADCAST_DEVICEINTERFACE_W dbi = {
        .dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE_W),
        .dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE,
        .dbcc_classguid = (GUID) { 0x4d1e55b2, 0xf16f, 0x11cf, { 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },
    };

    pWin32Platform->deviceNotificationHandle = RegisterDeviceNotificationW(
        pWin32Platform->helperWindowHandle,
        (DEV_BROADCAST_HDR*)&dbi,
        DEVICE_NOTIFY_WINDOW_HANDLE
    );

    if (!pWin32Platform->deviceNotificationHandle) {
        panic("Failed to register for device notifications");
    }

    MSG msg;
    while (PeekMessageW(&msg, pWin32Platform->helperWindowHandle, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return OK;
}

private Result _destroyHelperWindow(void) {
    Win32Platform* pWin32Platform = pDais->pPlatform->pInternal;

    if (pWin32Platform->deviceNotificationHandle) {
        if (UnregisterDeviceNotification(pWin32Platform->deviceNotificationHandle) == 0) {
            panic("Failed to unregister device notification");
        }
        pWin32Platform->deviceNotificationHandle = 0;
    }

    if (pWin32Platform->helperWindowHandle) {
        if (DestroyWindow(pWin32Platform->helperWindowHandle) == 0) {
            panic("Failed to destroy helper window");
        }
        pWin32Platform->helperWindowHandle = 0;
    }

    if (pWin32Platform->helperWindowClass) {
        if (UnregisterClassW(MAKEINTATOM(pWin32Platform->helperWindowClass), pWin32Platform->instance) == 0) {
            panic("Failed to unregister helper window class");
        }
        pWin32Platform->helperWindowClass = 0;
    }

    return OK;
}

public Result platformInit(Arena* pArena) {
    pDais->pPlatform = arenaPushStructZero(pArena, Platform);
    pDais->pPlatform->pInternal = arenaPushStructZero(pArena, Win32Platform);

    // Win32Platform* pWin32Platform = pDais->pPlatform->pInternal;

    try(_getModuleHandle());
    try(_createHelperWindow());

    _pollMonitors();

    return OK;
}

public Result platformRelease(void) {
    try(_destroyHelperWindow());

    return OK;
}

#endif
