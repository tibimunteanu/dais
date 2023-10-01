#include "base/base.h"

#if defined(OS_WINDOWS)

    #include "platform/win32/win32_base.h"
    #include "platform/win32/win32_platform_types.h"
    #include "core/log.h"
    #include "core/arena.h"

prv void _pollMonitors(void) {
    /* NOTE: how it works:

        assume we keep an array of connected monitors between calls
        create a disconnected array of all the monitor pointers

        loop display adapters
            skip if not active
            leave note to insert first or last depending on if its the primary adapter

            loop adapter monitors
                skip if not active
                if there is a monitor with this name is in the disconnected array
                    set the position in the disconnected array to null (not disconnected)
                    refresh the handle
                else
                    create a new monitor
                    insert it in the connected monitors array
                    call the MonitorConnected callback

            if the adapter does not have monitors, add the adapter as a monitor
                if there is an adapter as monitor with the same name in the disconnected array
                    set the position in the disconnected array to null (not disconnected)
                else
                    create a new adapter as monitor with no actual monitor
                    insert it in the m_Monitors array
                    call the MonitorConnected callback

        for each non null monitors in the disconnected array
            call the MonitorDisconnected callback

        delete the disconnected array of monitor pointers
     */

    logInfo("Polling monitors...");
}

prv LRESULT CALLBACK _helperWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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

prv Result _cacheInstanceHandle(void) {
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

prv Result _createHelperWindow(void) {
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
        .dbcc_classguid = (GUID) {0x4d1e55b2, 0xf16f, 0x11cf, {0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30}},
    };

    pWin32Platform->deviceNotificationHandle = RegisterDeviceNotificationW(
        pWin32Platform->helperWindowHandle, (DEV_BROADCAST_HDR*)&dbi, DEVICE_NOTIFY_WINDOW_HANDLE
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

prv Result _destroyHelperWindow(void) {
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

//
pub Result platformInit(Arena* pArena) {
    pDais->pPlatform = arenaPushStructZero(pArena, Platform);
    pDais->pPlatform->pInternal = arenaPushStructZero(pArena, Win32Platform);

    // Win32Platform* pWin32Platform = pDais->pPlatform->pInternal;

    try(_cacheInstanceHandle());
    try(_createHelperWindow());

    _pollMonitors();

    return OK;
}

pub Result platformRelease(void) {
    try(_destroyHelperWindow());

    return OK;
}

#endif /* if defined(OS_WINDOWS) */
