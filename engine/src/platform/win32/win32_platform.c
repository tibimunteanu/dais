#include "base/base.h"

#if defined(OS_WINDOWS)

    #include "platform/win32/win32_base.h"
    #include "platform/win32/win32_platform_types.h"
    #include "core/log.h"
    #include "core/arena.h"

prv Monitor* _createMonitor(DISPLAY_DEVICEW* pAdapter, DISPLAY_DEVICEW* pDisplay) {
    /* NOTE: how it works:

        save copies of both wide and utf8 converted name of (adapter) as adapter name
        save copies of both wide and utf8 converted name of (display ?? adapter) as monitor name

        save if modes are pruned from the adapter flags
        initialize mode changed to false

        get the device properties
        create a device context
        use the device properties and device context to get the width and height in millimeters
        delete the device context

        save the handle for the monitor

        return the monitor
    */
    return NULL;
}

prv fn _utf8FromWideString(Arena* pArena, const WCHAR* source, char** out_pResult) {
    U64 size = WideCharToMultiByte(CP_UTF8, 0, source, -1, NULL, 0, NULL, NULL);
    if (!size) {
        panic("Failed to convert string to UTF-8");
    }

    U64 arenaPos = pArena->pos;
    *out_pResult = arenaPushZero(pArena, size);

    if (!WideCharToMultiByte(CP_UTF8, 0, source, -1, *out_pResult, size, NULL, NULL)) {
        arenaPopTo(pArena, arenaPos);
        panic("Failed to convert string to UTF-8");
    }

    return OK;
}

prv fn _pollMonitors(void) {
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

        NOTE: the callbacks could actually be fire events. the platform layer
        listens for them and insert and delete monitors in the connected list
        and lets the events pass through to the game or anyone else might listen
     */

    logInfo("Polling monitors...");

    Platform* pPlatform = pDais->pPlatform;
    Pool* pPool = pPlatform->pMonitorPool;
    (void)poolAlloc(pPool);

    Arena* pArena = pPool->pArena;

    arenaTempBlock(pArena) {
        Monitor** disconnected =
            pPlatform->monitorCount
                ? arenaPushCopy(pArena, pPlatform->pMonitors, pPlatform->monitorCount * sizeof(Monitor*))
                : NULL;

        U32 disconnectedCount = pPlatform->monitorCount;

        for (U32 adapterIndex = 0;; adapterIndex++) {
            B8 insertFirst = false;

            DISPLAY_DEVICEW adapter = {
                .cb = sizeof(DISPLAY_DEVICEW),
            };

            if (!EnumDisplayDevicesW(NULL, adapterIndex, &adapter, 0)) {
                break;
            }

            if (!(adapter.StateFlags & DISPLAY_DEVICE_ACTIVE)) {
                continue;
            }

            if (adapter.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
                insertFirst = true;
            }

            U32 displayIndex;
            for (displayIndex = 0;; displayIndex++) {
                DISPLAY_DEVICEW display = {
                    .cb = sizeof(DISPLAY_DEVICEW),
                };

                if (!EnumDisplayDevicesW(adapter.DeviceName, displayIndex, &display, 0)) {
                    break;
                }

                if (!(display.StateFlags & DISPLAY_DEVICE_ACTIVE)) {
                    continue;
                }

                I32 i;
                for (i = 0; i < disconnectedCount; i++) {
                    if (disconnected[i] &&
                        wcscmp(((Win32Monitor*)disconnected[i]->pInternal)->displayName, display.DeviceName) == 0) {
                        disconnected[i] = NULL;
                        // TODO: handle may have changed, update
                        break;
                    }
                }

                if (i < disconnectedCount) {
                    continue;
                }

                // Monitor* pMonitor = _createMonitor(&adapter, &display);
                // if (!pMonitor) {
                //     return;
                // }

                // TODO: call monitor connected callback
                CString adapterName;
                CString displayName;
                try(_utf8FromWideString(pArena, adapter.DeviceString, &adapterName));
                try(_utf8FromWideString(pArena, display.DeviceString, &displayName));

                logInfo("Adapter: %s, Display: %s, %d", adapterName, displayName, insertFirst);

                insertFirst = false;
            }

            // HACK: if an active adapter does not have any display devices
            //       add it directly as a monitor
            if (displayIndex == 0) {
                I32 i;
                for (i = 0; i < disconnectedCount; i++) {
                    if (disconnected[i] &&
                        wcscmp(((Win32Monitor*)disconnected[i]->pInternal)->adapterName, adapter.DeviceName) == 0) {
                        disconnected[i] = NULL;
                        break;
                    }
                }

                if (i < disconnectedCount) {
                    continue;
                }

                // Monitor* monitor = _createMonitor(&adapter, NULL);
                // if (!monitor) {
                //     return;
                // }

                // TODO: call monitor connected callback
                CString adapterName;
                try(_utf8FromWideString(pArena, adapter.DeviceString, &adapterName));

                logInfo("Adapter: %s, Display: none", adapterName);
            }
        }

        for (I32 i = 0; i < disconnectedCount; i++) {
            if (disconnected[i]) {
                // TODO: call the monitor disconnected callback
            }
        }
    }

    return OK;
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

prv fn _cacheInstanceHandle(void) {
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

prv fn _createHelperWindow(void) {
    Win32Platform* pWin32Platform = pDais->pPlatform->pInternal;

    pWin32Platform->helperWindowClass = RegisterClassExW(&(WNDCLASSEXW) {
        .cbSize = sizeof(WNDCLASSEXW),
        .style = CS_OWNDC,
        .lpfnWndProc = (WNDPROC)_helperWindowProc,
        .hInstance = pWin32Platform->instance,
        .lpszClassName = L"DaisHelperWindow",
    });

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

prv fn _destroyHelperWindow(void) {
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
pub fn platformInit(Arena* pArena) {
    pDais->pPlatform = arenaPushStructZero(pArena, Platform);
    pDais->pPlatform->pInternal = arenaPushStructZero(pArena, Win32Platform);

    // Win32Platform* pWin32Platform = pDais->pPlatform->pInternal;

    try(_cacheInstanceHandle());
    try(_createHelperWindow());

    if (!pDais->pPlatform->pMonitorPool) {
        pDais->pPlatform->pMonitorPool = poolCreate(
            pArena,
            (PoolCreateInfo) {
                .slotSize = sizeof(Monitor),
                .slotAlignment = 1,
                .reservedSlots = 4,
            }
        );
    }

    try(_pollMonitors());

    return OK;
}

pub fn platformRelease(void) {
    try(_destroyHelperWindow());

    return OK;
}

#endif /* if defined(OS_WINDOWS) */
