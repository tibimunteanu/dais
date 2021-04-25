#include "platform/windows/WindowsPlatform.h"

namespace dais
{
    ThreadLocalStorage* ThreadLocalStorage::Create()
    {
        DWORD index = TlsAlloc();
        if (index == TLS_OUT_OF_INDEXES)
        {
            DAIS_ERROR("Failed to allocate TLS index!");
            return nullptr;
        }

        WindowsThreadLocalStorage* tls = new WindowsThreadLocalStorage();
        tls->m_Index = index;
        tls->m_Allocated = true;

        return tls;
    }

    WindowsThreadLocalStorage::~WindowsThreadLocalStorage()
    {
        if (m_Allocated)
        {
            TlsFree(m_Index);
        }
    }

    void* WindowsThreadLocalStorage::PlatformGet()
    {
        if (m_Allocated)
        {
            return TlsGetValue(m_Index);
        }
        return nullptr;
    }

    void WindowsThreadLocalStorage::PlatformSet(void* value)
    {
        if (m_Allocated)
        {
            TlsSetValue(m_Index, value);
        }
    }


    ////////////////////////////////////// STATIC MEMBERS /////////////////////////////////////////

    HWND WindowsPlatform::s_HelperWindowHandle = nullptr;
    HDEVNOTIFY WindowsPlatform::s_DeviceNotificationHandle = nullptr;
    DWORD WindowsPlatform::s_ForegroundLockTimeout = 0;
    int32_t WindowsPlatform::s_AcquiredMonitorCount = 0;
    char* WindowsPlatform::s_ClipboardString = nullptr;
    int16_t WindowsPlatform::s_Keycodes[] = {};
    int16_t WindowsPlatform::s_Scancodes[] = {};
    char WindowsPlatform::s_KeyNames[][5] = {};
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

        WindowsPlatform::CreateKeyTables();
        WindowsPlatform::UpdateKeyNames();

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

        WglContext::TerminateWGL();

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

    bool Platform::PlatformIsRawMouseMotionSupported()
    {
        return true;
    }

    const char* Platform::PlatformGetScancodeName(int32_t scancode)
    {
        if (scancode < 0
            || scancode >(KF_EXTENDED | 0xff)
            || WindowsPlatform::s_Keycodes[scancode] == DAIS_KEY_UNKNOWN)
        {
            DAIS_ERROR("Invalid scancode!");
            return nullptr;
        }
        return WindowsPlatform::s_KeyNames[WindowsPlatform::s_Keycodes[scancode]];
    }

    int32_t Platform::PlatformGetKeyScancode(int32_t key)
    {
        return WindowsPlatform::s_Scancodes[key];
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

    void WindowsPlatform::CreateKeyTables()
    {
        int32_t scancode;

        memset(s_Keycodes, -1, sizeof(s_Keycodes));
        memset(s_Scancodes, -1, sizeof(s_Scancodes));

        s_Keycodes[0x00B] = DAIS_KEY_0;
        s_Keycodes[0x002] = DAIS_KEY_1;
        s_Keycodes[0x003] = DAIS_KEY_2;
        s_Keycodes[0x004] = DAIS_KEY_3;
        s_Keycodes[0x005] = DAIS_KEY_4;
        s_Keycodes[0x006] = DAIS_KEY_5;
        s_Keycodes[0x007] = DAIS_KEY_6;
        s_Keycodes[0x008] = DAIS_KEY_7;
        s_Keycodes[0x009] = DAIS_KEY_8;
        s_Keycodes[0x00A] = DAIS_KEY_9;
        s_Keycodes[0x01E] = DAIS_KEY_A;
        s_Keycodes[0x030] = DAIS_KEY_B;
        s_Keycodes[0x02E] = DAIS_KEY_C;
        s_Keycodes[0x020] = DAIS_KEY_D;
        s_Keycodes[0x012] = DAIS_KEY_E;
        s_Keycodes[0x021] = DAIS_KEY_F;
        s_Keycodes[0x022] = DAIS_KEY_G;
        s_Keycodes[0x023] = DAIS_KEY_H;
        s_Keycodes[0x017] = DAIS_KEY_I;
        s_Keycodes[0x024] = DAIS_KEY_J;
        s_Keycodes[0x025] = DAIS_KEY_K;
        s_Keycodes[0x026] = DAIS_KEY_L;
        s_Keycodes[0x032] = DAIS_KEY_M;
        s_Keycodes[0x031] = DAIS_KEY_N;
        s_Keycodes[0x018] = DAIS_KEY_O;
        s_Keycodes[0x019] = DAIS_KEY_P;
        s_Keycodes[0x010] = DAIS_KEY_Q;
        s_Keycodes[0x013] = DAIS_KEY_R;
        s_Keycodes[0x01F] = DAIS_KEY_S;
        s_Keycodes[0x014] = DAIS_KEY_T;
        s_Keycodes[0x016] = DAIS_KEY_U;
        s_Keycodes[0x02F] = DAIS_KEY_V;
        s_Keycodes[0x011] = DAIS_KEY_W;
        s_Keycodes[0x02D] = DAIS_KEY_X;
        s_Keycodes[0x015] = DAIS_KEY_Y;
        s_Keycodes[0x02C] = DAIS_KEY_Z;

        s_Keycodes[0x028] = DAIS_KEY_APOSTROPHE;
        s_Keycodes[0x02B] = DAIS_KEY_BACKSLASH;
        s_Keycodes[0x033] = DAIS_KEY_COMMA;
        s_Keycodes[0x00D] = DAIS_KEY_EQUAL;
        s_Keycodes[0x029] = DAIS_KEY_GRAVE_ACCENT;
        s_Keycodes[0x01A] = DAIS_KEY_LEFT_BRACKET;
        s_Keycodes[0x00C] = DAIS_KEY_MINUS;
        s_Keycodes[0x034] = DAIS_KEY_PERIOD;
        s_Keycodes[0x01B] = DAIS_KEY_RIGHT_BRACKET;
        s_Keycodes[0x027] = DAIS_KEY_SEMICOLON;
        s_Keycodes[0x035] = DAIS_KEY_SLASH;
        s_Keycodes[0x056] = DAIS_KEY_WORLD_2;

        s_Keycodes[0x00E] = DAIS_KEY_BACKSPACE;
        s_Keycodes[0x153] = DAIS_KEY_DELETE;
        s_Keycodes[0x14F] = DAIS_KEY_END;
        s_Keycodes[0x01C] = DAIS_KEY_ENTER;
        s_Keycodes[0x001] = DAIS_KEY_ESCAPE;
        s_Keycodes[0x147] = DAIS_KEY_HOME;
        s_Keycodes[0x152] = DAIS_KEY_INSERT;
        s_Keycodes[0x15D] = DAIS_KEY_MENU;
        s_Keycodes[0x151] = DAIS_KEY_PAGE_DOWN;
        s_Keycodes[0x149] = DAIS_KEY_PAGE_UP;
        s_Keycodes[0x045] = DAIS_KEY_PAUSE;
        s_Keycodes[0x146] = DAIS_KEY_PAUSE;
        s_Keycodes[0x039] = DAIS_KEY_SPACE;
        s_Keycodes[0x00F] = DAIS_KEY_TAB;
        s_Keycodes[0x03A] = DAIS_KEY_CAPS_LOCK;
        s_Keycodes[0x145] = DAIS_KEY_NUM_LOCK;
        s_Keycodes[0x046] = DAIS_KEY_SCROLL_LOCK;
        s_Keycodes[0x03B] = DAIS_KEY_F1;
        s_Keycodes[0x03C] = DAIS_KEY_F2;
        s_Keycodes[0x03D] = DAIS_KEY_F3;
        s_Keycodes[0x03E] = DAIS_KEY_F4;
        s_Keycodes[0x03F] = DAIS_KEY_F5;
        s_Keycodes[0x040] = DAIS_KEY_F6;
        s_Keycodes[0x041] = DAIS_KEY_F7;
        s_Keycodes[0x042] = DAIS_KEY_F8;
        s_Keycodes[0x043] = DAIS_KEY_F9;
        s_Keycodes[0x044] = DAIS_KEY_F10;
        s_Keycodes[0x057] = DAIS_KEY_F11;
        s_Keycodes[0x058] = DAIS_KEY_F12;
        s_Keycodes[0x064] = DAIS_KEY_F13;
        s_Keycodes[0x065] = DAIS_KEY_F14;
        s_Keycodes[0x066] = DAIS_KEY_F15;
        s_Keycodes[0x067] = DAIS_KEY_F16;
        s_Keycodes[0x068] = DAIS_KEY_F17;
        s_Keycodes[0x069] = DAIS_KEY_F18;
        s_Keycodes[0x06A] = DAIS_KEY_F19;
        s_Keycodes[0x06B] = DAIS_KEY_F20;
        s_Keycodes[0x06C] = DAIS_KEY_F21;
        s_Keycodes[0x06D] = DAIS_KEY_F22;
        s_Keycodes[0x06E] = DAIS_KEY_F23;
        s_Keycodes[0x076] = DAIS_KEY_F24;
        s_Keycodes[0x038] = DAIS_KEY_LEFT_ALT;
        s_Keycodes[0x01D] = DAIS_KEY_LEFT_CONTROL;
        s_Keycodes[0x02A] = DAIS_KEY_LEFT_SHIFT;
        s_Keycodes[0x15B] = DAIS_KEY_LEFT_SUPER;
        s_Keycodes[0x137] = DAIS_KEY_PRINT_SCREEN;
        s_Keycodes[0x138] = DAIS_KEY_RIGHT_ALT;
        s_Keycodes[0x11D] = DAIS_KEY_RIGHT_CONTROL;
        s_Keycodes[0x036] = DAIS_KEY_RIGHT_SHIFT;
        s_Keycodes[0x15C] = DAIS_KEY_RIGHT_SUPER;
        s_Keycodes[0x150] = DAIS_KEY_DOWN;
        s_Keycodes[0x14B] = DAIS_KEY_LEFT;
        s_Keycodes[0x14D] = DAIS_KEY_RIGHT;
        s_Keycodes[0x148] = DAIS_KEY_UP;

        s_Keycodes[0x052] = DAIS_KEY_KP_0;
        s_Keycodes[0x04F] = DAIS_KEY_KP_1;
        s_Keycodes[0x050] = DAIS_KEY_KP_2;
        s_Keycodes[0x051] = DAIS_KEY_KP_3;
        s_Keycodes[0x04B] = DAIS_KEY_KP_4;
        s_Keycodes[0x04C] = DAIS_KEY_KP_5;
        s_Keycodes[0x04D] = DAIS_KEY_KP_6;
        s_Keycodes[0x047] = DAIS_KEY_KP_7;
        s_Keycodes[0x048] = DAIS_KEY_KP_8;
        s_Keycodes[0x049] = DAIS_KEY_KP_9;
        s_Keycodes[0x04E] = DAIS_KEY_KP_ADD;
        s_Keycodes[0x053] = DAIS_KEY_KP_DECIMAL;
        s_Keycodes[0x135] = DAIS_KEY_KP_DIVIDE;
        s_Keycodes[0x11C] = DAIS_KEY_KP_ENTER;
        s_Keycodes[0x059] = DAIS_KEY_KP_EQUAL;
        s_Keycodes[0x037] = DAIS_KEY_KP_MULTIPLY;
        s_Keycodes[0x04A] = DAIS_KEY_KP_SUBTRACT;

        for (scancode = 0; scancode < 512; scancode++)
        {
            if (s_Keycodes[scancode] > 0)
            {
                s_Scancodes[s_Keycodes[scancode]] = scancode;
            }
        }
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

    /// <summary> Updates key names according to the current keyboard layout </summary>
    void WindowsPlatform::UpdateKeyNames()
    {
        memset(s_KeyNames, 0, sizeof(s_KeyNames));

        for (int32_t key = DAIS_KEY_SPACE; key <= DAIS_KEY_LAST; key++)
        {
            int32_t scancode = s_Scancodes[key];
            if (scancode == -1)
            {
                continue;
            }

            UINT vk;
            if (key >= DAIS_KEY_KP_0
                && key <= DAIS_KEY_KP_ADD)
            {
                const UINT vks[] =
                {
                    VK_NUMPAD0,  VK_NUMPAD1,  VK_NUMPAD2, VK_NUMPAD3,
                    VK_NUMPAD4,  VK_NUMPAD5,  VK_NUMPAD6, VK_NUMPAD7,
                    VK_NUMPAD8,  VK_NUMPAD9,  VK_DECIMAL, VK_DIVIDE,
                    VK_MULTIPLY, VK_SUBTRACT, VK_ADD
                };

                vk = vks[key - DAIS_KEY_KP_0];
            }
            else
            {
                vk = MapVirtualKey(scancode, MAPVK_VSC_TO_VK);
            }

            WCHAR chars[16];
            BYTE state[256] = { 0 };
            int length = ToUnicode(vk, scancode, state, chars, sizeof(chars) / sizeof(WCHAR), 0);
            if (length == -1)
            {
                length = ToUnicode(vk, scancode, state, chars, sizeof(chars) / sizeof(WCHAR), 0);
            }

            if (length < 1)
            {
                continue;
            }

            WideCharToMultiByte(CP_UTF8, 0, chars, 1,
                s_KeyNames[key],
                sizeof(s_KeyNames[key]),
                NULL, NULL);
        }
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
