#include "platform/windows/WindowsPlatform.h"

namespace dais
{
    ////////////////////////////////////// STATIC MEMBERS /////////////////////////////////////////

    HWND WindowsPlatform::s_HelperWindowHandle = nullptr;
    HDEVNOTIFY WindowsPlatform::s_DeviceNotificationHandle = nullptr;
    DWORD WindowsPlatform::s_ForegroundLockTimeout = 0;
    int32_t WindowsPlatform::s_AcquiredMonitorCount = 0;
    char* WindowsPlatform::s_ClipboardString = nullptr;
    double WindowsPlatform::s_RestoreCursorPositionX = 0.0;
    double WindowsPlatform::s_RestoreCursorPositionY = 0.0;
    Window* WindowsPlatform::s_DisabledCursorWindow = nullptr;
    RAWINPUT* WindowsPlatform::s_RawInput = nullptr;
    int32_t WindowsPlatform::s_RawInputSize = 0;
    UINT WindowsPlatform::s_MouseTrailSize = 0;
    WindowsPlatform::WindowsLibs WindowsPlatform::s_Libs = {};



    //////////////////////////////////////// STATIC API ///////////////////////////////////////////

    bool Platform::PlatformInit()
    {
        WindowsPlatform::SetForegroundLockTimeout();

        if (!WindowsPlatform::LoadLibraries())
        {
            return false;
        }

        WindowsPlatform::SetProcessDpiAware();

        if (!WindowsPlatform::RegisterWindowClass())
        {
            return false;
        }

        if (!WindowsPlatform::CreateHelperWindow())
        {
            return false;
        }

        WindowsPlatform::PollMonitors();

        return true;
    }

    void Platform::PlatformTerminate()
    {
        if (WindowsPlatform::s_DeviceNotificationHandle)
        {
            UnregisterDeviceNotification(WindowsPlatform::s_DeviceNotificationHandle);
        }

        if (WindowsPlatform::s_HelperWindowHandle)
        {
            DestroyWindow(WindowsPlatform::s_HelperWindowHandle);
        }

        WindowsPlatform::UnregisterWindowClass();
        WindowsPlatform::RestoreForegroundLockTimeout();

        WindowsPlatform::FreeLibraries();
    }


    void Platform::PlatformPollEvents()
    {
        MSG msg = {};

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                //this message can only be posted from outside so treat it like a full close
                DAIS_INFO("WM_QUIT");
                Platform::Terminate();
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }

    void Platform::PlatformWaitEvents()
    {
        WaitMessage();
        PollEvents();
    }

    void Platform::PlatformWaitEventsTimeout(double timeout)
    {
        if (timeout != timeout
            || timeout < 0.0
            || timeout > DBL_MAX)
        {
            DAIS_ERROR("Invalid time %f", timeout);
            return;
        }

        MsgWaitForMultipleObjects(0, NULL, FALSE, (DWORD)(timeout * 1000.0), QS_ALLEVENTS);
        PollEvents();
    }

    bool Platform::IsRawMouseMotionSupported()
    {
        return true;
    }


    bool WindowsPlatform::LoadLibraries()
    {
        DAIS_TRACE("[WindowsPlatform] Loading libraries");

        //XInput
        {
            int i;
            const char* names[] =
            {
                "xinput1_4.dll",
                "xinput1_3.dll",
                "xinput9_1_0.dll",
                "xinput1_2.dll",
                "xinput1_1.dll",
                nullptr
            };

            for (i = 0; names[i]; i++)
            {
                s_Libs.xInput.instance = LoadLibraryA(names[i]);
                if (s_Libs.xInput.instance)
                {
                    s_Libs.xInput.GetCapabilities = (PFN_XInputGetCapabilities)GetProcAddress(s_Libs.xInput.instance, "XInputGetCapabilities");
                    s_Libs.xInput.GetState = (PFN_XInputGetState)GetProcAddress(s_Libs.xInput.instance, "XInputGetState");
                    break;
                }
            }
        }

        s_Libs.dInput8.instance = LoadLibraryA("dinput8.dll");
        if (s_Libs.dInput8.instance)
        {
            s_Libs.dInput8.Create = (PFN_DirectInput8Create)GetProcAddress(s_Libs.dInput8.instance, "DirectInput8Create");
        }

        s_Libs.winmm.instance = LoadLibraryA("winmm.dll");
        if (!s_Libs.winmm.instance)
        {
            DAIS_ERROR("[WindowsPlatform] LoadLibraries(): Failed to load winmm.dll!");
            return false;
        }

        s_Libs.winmm.GetTime = (PFN_timeGetTime)GetProcAddress(s_Libs.winmm.instance, "timeGetTime");

        s_Libs.user32.instance = LoadLibraryA("user32.dll");
        if (!s_Libs.user32.instance)
        {
            DAIS_ERROR("[WindowsPlatform] LoadLibraries(): Failed to load user32.dll!");
            return false;
        }

        s_Libs.user32.SetProcessDPIAware = (PFN_SetProcessDPIAware)GetProcAddress(s_Libs.user32.instance, "SetProcessDPIAware");
        s_Libs.user32.ChangeWindowMessageFilterEx = (PFN_ChangeWindowMessageFilterEx)GetProcAddress(s_Libs.user32.instance, "ChangeWindowMessageFilterEx");
        s_Libs.user32.EnableNonClientDpiScaling = (PFN_EnableNonClientDpiScaling)GetProcAddress(s_Libs.user32.instance, "EnableNonClientDpiScaling");
        s_Libs.user32.SetProcessDpiAwarenessContext = (PFN_SetProcessDpiAwarenessContext)GetProcAddress(s_Libs.user32.instance, "SetProcessDpiAwarenessContext");
        s_Libs.user32.GetDpiForWindow = (PFN_GetDpiForWindow)GetProcAddress(s_Libs.user32.instance, "GetDpiForWindow");
        s_Libs.user32.AdjustWindowRectExForDpi = (PFN_AdjustWindowRectExForDpi)GetProcAddress(s_Libs.user32.instance, "AdjustWindowRectExForDpi");

        s_Libs.dwmapi.instance = LoadLibraryA("dwmapi.dll");
        if (s_Libs.dwmapi.instance)
        {
            s_Libs.dwmapi.IsCompositionEnabled = (PFN_DwmIsCompositionEnabled)GetProcAddress(s_Libs.dwmapi.instance, "DwmIsCompositionEnabled");
            s_Libs.dwmapi.Flush = (PFN_DwmFlush)GetProcAddress(s_Libs.dwmapi.instance, "DwmFlush");
            s_Libs.dwmapi.EnableBlurBehindWindow = (PFN_DwmEnableBlurBehindWindow)GetProcAddress(s_Libs.dwmapi.instance, "DwmEnableBlurBehindWindow");
            s_Libs.dwmapi.GetColorizationColor = (PFN_DwmGetColorizationColor)GetProcAddress(s_Libs.dwmapi.instance, "DwmGetColorizationColor");
        }

        s_Libs.shcore.instance = LoadLibraryA("shcore.dll");
        if (s_Libs.shcore.instance)
        {
            s_Libs.shcore.SetProcessDpiAwareness = (PFN_SetProcessDpiAwareness)GetProcAddress(s_Libs.shcore.instance, "SetProcessDpiAwareness");
            s_Libs.shcore.GetDpiForMonitor = (PFN_GetDpiForMonitor)GetProcAddress(s_Libs.shcore.instance, "GetDpiForMonitor");
        }

        s_Libs.ntdll.instance = LoadLibraryA("ntdll.dll");
        if (s_Libs.ntdll.instance)
        {
            s_Libs.ntdll.RtlVerifyVersionInfo = (PFN_RtlVerifyVersionInfo)GetProcAddress(s_Libs.ntdll.instance, "RtlVerifyVersionInfo");
        }

        return true;
    }

    void WindowsPlatform::FreeLibraries()
    {
        DAIS_TRACE("[WindowsPlatform] Free libraries");

        if (s_Libs.xInput.instance) FreeLibrary(s_Libs.xInput.instance);
        if (s_Libs.dInput8.instance) FreeLibrary(s_Libs.dInput8.instance);
        if (s_Libs.winmm.instance) FreeLibrary(s_Libs.winmm.instance);
        if (s_Libs.user32.instance) FreeLibrary(s_Libs.user32.instance);
        if (s_Libs.dwmapi.instance) FreeLibrary(s_Libs.dwmapi.instance);
        if (s_Libs.shcore.instance) FreeLibrary(s_Libs.shcore.instance);
        if (s_Libs.ntdll.instance) FreeLibrary(s_Libs.ntdll.instance);
    }

    void WindowsPlatform::SetProcessDpiAware()
    {
        if (IsWindows10CreatorsUpdateOrGreater())
        {
            s_Libs.user32.SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        }
        else if (IsWindows8Point1OrGreater())
        {
            s_Libs.shcore.SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
        }
        else if (IsWindowsVistaOrGreater())
        {
            s_Libs.user32.SetProcessDPIAware();
        }
    }

    void WindowsPlatform::SetForegroundLockTimeout()
    {
        SystemParametersInfoW(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &s_ForegroundLockTimeout, 0);
        SystemParametersInfoW(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, UIntToPtr(0), SPIF_SENDCHANGE);
    }

    void WindowsPlatform::RestoreForegroundLockTimeout()
    {
        SystemParametersInfoW(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, UIntToPtr(s_ForegroundLockTimeout), SPIF_SENDCHANGE);
    }

    void WindowsPlatform::AdjustRect(RECT* rect, HWND windowHandle, DWORD style, DWORD styleEx)
    {
        if (IsWindows10AnniversaryUpdateOrGreater())
        {
            UINT dpi = s_Libs.user32.GetDpiForWindow(windowHandle);
            s_Libs.user32.AdjustWindowRectExForDpi(rect, style, FALSE, styleEx, dpi);
        }
        else
        {
            AdjustWindowRectEx(rect, style, FALSE, styleEx);
        }
    }

    void WindowsPlatform::AdjustRect(RECT* rect, DWORD style, DWORD styleEx, UINT dpi)
    {
        if (IsWindows10AnniversaryUpdateOrGreater())
        {
            s_Libs.user32.AdjustWindowRectExForDpi(rect, style, FALSE, styleEx, dpi);
        }
        else
        {
            AdjustWindowRectEx(rect, style, FALSE, styleEx);
        }
    }

    void WindowsPlatform::PollMonitors()
    {
        DAIS_TRACE("[WindowsPlatform] PollMonitors");

        //copy the array of pointers to the monitors
        std::vector<Monitor*> disconnected = s_Monitors;

        //loop display adapters
        uint32_t adapterIndex;
        for (adapterIndex = 0; ; adapterIndex++)
        {
            bool insertFirst = false;

            DISPLAY_DEVICEW adapter = {};
            adapter.cb = sizeof(adapter);

            if (!EnumDisplayDevicesW(NULL, adapterIndex, &adapter, 0))
            {
                break;
            }

            if (!(adapter.StateFlags & DISPLAY_DEVICE_ACTIVE))
            {
                continue;
            }

            if (adapter.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
            {
                insertFirst = true;
            }

            //loop adapter monitors
            uint32_t displayIndex;
            for (displayIndex = 0; ; displayIndex++)
            {
                DISPLAY_DEVICEW display = {};
                display.cb = sizeof(display);

                if (!EnumDisplayDevicesW(adapter.DeviceName, displayIndex, &display, 0))
                {
                    break;
                }

                if (!(display.StateFlags & DISPLAY_DEVICE_ACTIVE))
                {
                    continue;
                }

                //if this monitor is in the disconnected array, set the position as null and just refresh the handle
                uint32_t d;
                for (d = 0; d < disconnected.size(); d++)
                {
                    if (disconnected[d]
                        && wcscmp(((WindowsMonitor*)disconnected[d])->m_DisplayName, display.DeviceName) == 0)
                    {
                        disconnected[d] = nullptr;

                        EnumDisplayMonitors(nullptr, nullptr, WindowsMonitor::SetHandle, (LPARAM)s_Monitors[d]);
                        break;
                    }
                }

                if (d < disconnected.size())
                {
                    continue;
                }

                //if the monitor didn't already exist, create a new one
                WindowsMonitor* monitor = new WindowsMonitor(&adapter, &display);
                if (!monitor)
                {
                    return;
                }

                //insert it in the m_Monitors array
                if (insertFirst)
                {
                    s_Monitors.insert(s_Monitors.begin(), monitor);
                }
                else
                {
                    s_Monitors.push_back(monitor);
                }
                insertFirst = false;

                //call the MonitorConnected callback
                if (s_Callbacks.monitorConnected)
                {
                    s_Callbacks.monitorConnected((Monitor*)monitor);
                }
            }

            //if an active adapter does not have any display devices add it as a monitor
            if (displayIndex == 0)
            {
                //if this adapter (as monitor) is in the disconnected array, set the position as null and just refresh the handle
                uint32_t d;
                for (d = 0; d < disconnected.size(); d++)
                {
                    if (disconnected[d]
                        && wcscmp(((WindowsMonitor*)disconnected[d])->m_DisplayName, adapter.DeviceName) == 0)
                    {
                        disconnected[d] = nullptr;
                        break;
                    }
                }

                if (d < disconnected.size())
                {
                    continue;
                }

                //if the monitor didn't already exist, create a new one
                WindowsMonitor* monitor = new WindowsMonitor(&adapter, nullptr);
                if (!monitor)
                {
                    return;
                }

                //insert it in the m_Monitors array
                s_Monitors.push_back(monitor);

                //call the MonitorConnected callback
                if (s_Callbacks.monitorConnected)
                {
                    s_Callbacks.monitorConnected((Monitor*)monitor);
                }
            }
        }

        //release remaining monitors in the disconnected array since they were not found
        for (uint32_t d = 0; d < disconnected.size(); d++)
        {
            if (disconnected[d])
            {
                Monitor* monitor = s_Monitors[d];

                for (Window* window : s_Windows)
                {
                    if (window->GetMonitor() == monitor)
                    {
                        int32_t width, height;
                        window->GetSize(&width, &height);
                        window->SetMonitor(nullptr, 0, 0, width, height, 0);

                        int32_t xOffset, yOffset;
                        window->GetFrameSize(&xOffset, &yOffset, nullptr, nullptr);
                        window->SetPosition(xOffset, yOffset);
                    }
                }

                //remove this monitor from the m_Monitors array
                s_Monitors.erase(s_Monitors.begin() + d);

                //also remove this monitor from the disconnected array to keep indices in sync with m_Monitors array
                disconnected.erase(disconnected.begin() + d);

                //call the MonitorDisconnected callback
                if (s_Callbacks.monitorDisconnected)
                {
                    s_Callbacks.monitorDisconnected((Monitor*)monitor);
                }

                delete monitor;
            }
        }
    }

    bool WindowsPlatform::RegisterWindowClass()
    {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(wc);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = WindowsWindow::WindowProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.lpszClassName = DAIS_WINDOW_CLASS;

        //load user provided icon if available
        wc.hIcon = (HICON)LoadImageW(GetModuleHandleW(nullptr), DAIS_ICON, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
        if (!wc.hIcon)
        {
            //no user provided icon found, load default icon
            wc.hIcon = (HICON)LoadImageW(nullptr, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
        }

        if (!RegisterClassExW(&wc))
        {
            DAIS_ERROR("Failed to register window class!");
            return false;
        }

        return true;
    }

    void WindowsPlatform::UnregisterWindowClass()
    {
        UnregisterClassW(DAIS_WINDOW_CLASS, GetModuleHandleW(nullptr));
    }

    bool WindowsPlatform::CreateHelperWindow()
    {
        s_HelperWindowHandle = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW,
            DAIS_WINDOW_CLASS,
            DAIS_HELPER_WINDOW_TITLE,
            WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            0, 0, 1, 1,
            NULL, NULL,
            GetModuleHandleW(NULL),
            NULL);

        if (!s_HelperWindowHandle)
        {
            DAIS_TRACE("Failed to create helper window!");
            return false;
        }

        // HACK: the command to the first ShowWindow call is ignored if the parent
        // process passed along a STARTUPINFO, so clear that with a no-op call
        ShowWindow(s_HelperWindowHandle, SW_HIDE);

        //register for HID device notifications
        DEV_BROADCAST_DEVICEINTERFACE_W dbi = {};
        dbi.dbcc_size = sizeof(dbi);
        dbi.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        dbi.dbcc_classguid = GUID_DEVINTERFACE_HID;

        s_DeviceNotificationHandle = RegisterDeviceNotificationW(s_HelperWindowHandle,
            (DEV_BROADCAST_HDR*)&dbi,
            DEVICE_NOTIFY_WINDOW_HANDLE);

        //process messages
        MSG msg;
        while (PeekMessageW(&msg, s_HelperWindowHandle, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        return true;
    }


    bool WindowsPlatform::IsWindowsVersionOrGreater(WORD major, WORD minor, WORD sp)
    {
        OSVERSIONINFOEXW osvi = { sizeof(osvi), major, minor, 0, 0, {0}, sp };
        DWORD mask = VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR;
        ULONGLONG cond = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
        cond = VerSetConditionMask(cond, VER_MINORVERSION, VER_GREATER_EQUAL);
        cond = VerSetConditionMask(cond, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
        // HACK: Use RtlVerifyVersionInfo instead of VerifyVersionInfoW as the
        //       latter lies unless the user knew to embed a non-default manifest
        //       announcing support for Windows 10 via supportedOS GUID
        return s_Libs.ntdll.RtlVerifyVersionInfo(&osvi, mask, cond) == 0;
    }

    bool WindowsPlatform::IsWindowsVistaOrGreater()
    {
        return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 0);
    }

    bool WindowsPlatform::IsWindows7OrGreater()
    {
        return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 0);
    }

    bool WindowsPlatform::IsWindows8OrGreater()
    {
        return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN8), LOBYTE(_WIN32_WINNT_WIN8), 0);
    }

    bool WindowsPlatform::IsWindows8Point1OrGreater()
    {
        return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINBLUE), LOBYTE(_WIN32_WINNT_WINBLUE), 0);
    }

    bool WindowsPlatform::IsWindows10AnniversaryUpdateOrGreater()
    {
        return IsWindows10BuildOrGreater(14393);
    }

    bool WindowsPlatform::IsWindows10CreatorsUpdateOrGreater()
    {
        return IsWindows10BuildOrGreater(15063);
    }

    bool WindowsPlatform::IsWindows10BuildOrGreater(WORD build)
    {
        // Checks whether we are on at least the specified build of Windows 10
        OSVERSIONINFOEXW osvi = { sizeof(osvi), 10, 0, build };
        DWORD mask = VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER;
        ULONGLONG cond = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
        cond = VerSetConditionMask(cond, VER_MINORVERSION, VER_GREATER_EQUAL);
        cond = VerSetConditionMask(cond, VER_BUILDNUMBER, VER_GREATER_EQUAL);
        // HACK: Use RtlVerifyVersionInfo instead of VerifyVersionInfoW as the
        //       latter lies unless the user knew to embed a non-default manifest
        //       announcing support for Windows 10 via supportedOS GUID
        return s_Libs.ntdll.RtlVerifyVersionInfo(&osvi, mask, cond) == 0;
    }


    char* WindowsPlatform::WideStringToUTF8(const WCHAR* source)
    {
        char* target;
        int size;

        size = WideCharToMultiByte(CP_UTF8, 0, source, -1, nullptr, 0, nullptr, nullptr);
        if (!size)
        {
            return nullptr;
        }

        target = (char*)calloc(size, 1);

        if (!WideCharToMultiByte(CP_UTF8, 0, source, -1, target, size, nullptr, nullptr))
        {
            free(target);
            return nullptr;
        }

        return target;
    }

    bool WindowsPlatform::WideStringToUTF8(const WCHAR* source, std::string& target)
    {
        char* name = WideStringToUTF8(source);
        if (name)
        {
            target = std::string(name);
            free(name);
            return true;
        }
        return false;
    }

    bool WindowsPlatform::WideStringToUTF8(const WCHAR source[], char target[])
    {
        return WideCharToMultiByte(CP_UTF8, 0, source, -1, target, sizeof(target), nullptr, nullptr);
    }

    WCHAR* WindowsPlatform::UTF8ToWideString(const char* source)
    {
        WCHAR* target;
        int count;

        count = MultiByteToWideChar(CP_UTF8, 0, source, -1, NULL, 0);
        if (!count)
        {
            return nullptr;
        }

        target = (WCHAR*)calloc(count, sizeof(WCHAR));

        if (!MultiByteToWideChar(CP_UTF8, 0, source, -1, target, count))
        {
            free(target);
            return nullptr;
        }

        return target;
    }
}
