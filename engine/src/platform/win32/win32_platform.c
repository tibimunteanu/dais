#include "base/base.h"

#ifdef OS_WINDOWS
#    include "platform/win32/win32_base.h"
#    include "platform/win32/win32_platform_types.h"
#    include "core/log.h"
#    include "core/arena.h"

// private
internal void _pollMonitors(void) {
    logInfo("Polling monitors...");
}

internal LRESULT CALLBACK _helperWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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

internal Result _createHelperWindow(void) {
    Win32Platform* pWin32Platform = pDais->pPlatform->pInternal;

    MSG msg;
    WNDCLASSEXW wc = {sizeof(wc)};
    HMODULE module = GetModuleHandle(0);

    wc.style = CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC)_helperWindowProc;
    wc.hInstance = module;
    wc.lpszClassName = L"DaisHelperWindow";

    pWin32Platform->helperWindowClass = RegisterClassExW(&wc);
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
        module,
        NULL
    );

    if (!pWin32Platform->helperWindowHandle) {
        panic("Failed to create helper window");
    }

    // NOTE: if a STARTUPINFO is passed along, the first ShowWindow is ignored
    ShowWindow(pWin32Platform->helperWindowHandle, SW_HIDE);

    // register for HID device notifications
    {
        DEV_BROADCAST_DEVICEINTERFACE_W dbi = {
            .dbcc_size = sizeof(dbi),
            .dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE,
            .dbcc_classguid = (GUID) {0x4d1e55b2, 0xf16f, 0x11cf, {0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30}},
        };

        pWin32Platform->deviceNotificationHandle = RegisterDeviceNotificationW(
            pWin32Platform->helperWindowHandle, (DEV_BROADCAST_HDR*)&dbi, DEVICE_NOTIFY_WINDOW_HANDLE
        );
    }

    while (PeekMessageW(&msg, pWin32Platform->helperWindowHandle, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return OK;
}

// init
Result platformInit(Arena* pArena) {
    pDais->pPlatform = arenaPushStructZero(pArena, Platform);
    pDais->pPlatform->pInternal = arenaPushStructZero(pArena, Win32Platform);

    Win32Platform* pWin32Platform = pDais->pPlatform->pInternal;

    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (const WCHAR*)&pDais,
            (HMODULE*)&pWin32Platform->instance
        )) {
        panic("Failed to get the module handle");
    }

    try(_createHelperWindow());

    _pollMonitors();

    return OK;
}

Result platformRelease(void) {
    return OK;
}

#endif
