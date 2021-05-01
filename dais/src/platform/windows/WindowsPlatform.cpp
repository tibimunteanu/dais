#include "platform/windows/WindowsPlatform.h"

namespace dais
{
    ////////////////////////////////////// STATIC INIT ////////////////////////////////////////////

    HWND WindowsPlatform::s_HelperWindowHandle = nullptr;
    HDEVNOTIFY WindowsPlatform::s_DeviceNotificationHandle = nullptr;
    DWORD WindowsPlatform::s_ForegroundLockTimeout = 0;
    int32_t WindowsPlatform::s_AcquiredMonitorCount = 0;
    char* WindowsPlatform::s_ClipboardString = nullptr;
    Key WindowsPlatform::s_Keycodes[] = {};
    int16_t WindowsPlatform::s_Scancodes[] = {};
    char WindowsPlatform::s_KeyNames[][5] = {};
    double WindowsPlatform::s_RestoreCursorPositionX = 0.0;
    double WindowsPlatform::s_RestoreCursorPositionY = 0.0;
    Window* WindowsPlatform::s_DisabledCursorWindow = nullptr;
    RAWINPUT* WindowsPlatform::s_RawInput = nullptr;
    int32_t WindowsPlatform::s_RawInputSize = 0;
    UINT WindowsPlatform::s_MouseTrailSize = 0;
    WindowsPlatform::WindowsLibs WindowsPlatform::s_Libs = {};
    std::vector<std::string> WindowsPlatform::s_EglLibNames = { "libEGL.dll", "EGL.dll" };
    std::vector<std::string> WindowsPlatform::s_GLES1LibNames = { "GLESv1_CM.dll", "libGLES_CM.dll" };
    std::vector<std::string> WindowsPlatform::s_GLES2LibNames = { "GLESv2.dll", "libGLESv2.dll" };
    std::vector<std::string> WindowsPlatform::s_GLSLibNames = { };



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

        WindowsWglContext::Terminate();

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
            || WindowsPlatform::s_Keycodes[scancode] == Key::Unknown)
        {
            DAIS_ERROR("Invalid scancode!");
            return nullptr;
        }
        return WindowsPlatform::s_KeyNames[(int32_t)WindowsPlatform::s_Keycodes[scancode]];
    }

    int32_t Platform::PlatformGetKeyScancode(Key key)
    {
        return WindowsPlatform::s_Scancodes[(int32_t)key];
    }


    const std::vector<std::string>& Platform::PlatformGetEglLibNames()
    {
        return WindowsPlatform::s_EglLibNames;
    }

    const std::vector<std::string>& Platform::PlatformGetGLES1LibNames()
    {
        return WindowsPlatform::s_GLES1LibNames;
    }

    const std::vector<std::string>& Platform::PlatformGetGLES2LibNames()
    {
        return WindowsPlatform::s_GLES2LibNames;
    }

    const std::vector<std::string>& Platform::PlatformGetGLSLibNames()
    {
        return WindowsPlatform::s_GLSLibNames;
    }


    EGLenum Platform::PlatformGetEglPlatform(EGLint** attribs)
    {
        if (EglContext::s_EGL.ANGLE_PlatformAngle)
        {
            int type = 0;

            if (EglContext::s_EGL.ANGLE_PlatformAngleOpenGL)
            {
                if (s_Hints.init.angleType == AnglePlatformType::OpenGL)
                {
                    type = EGL_PLATFORM_ANGLE_TYPE_OPENGL_ANGLE;
                }
                else if (s_Hints.init.angleType == AnglePlatformType::OpenGLES)
                {
                    type = EGL_PLATFORM_ANGLE_TYPE_OPENGLES_ANGLE;
                }
            }

            if (EglContext::s_EGL.ANGLE_PlatformAngleD3D)
            {
                if (s_Hints.init.angleType == AnglePlatformType::D3D9)
                {
                    type = EGL_PLATFORM_ANGLE_TYPE_D3D9_ANGLE;
                }
                else if (s_Hints.init.angleType == AnglePlatformType::D3D11)
                {
                    type = EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE;
                }
            }

            if (EglContext::s_EGL.ANGLE_PlatformAngleVulkan)
            {
                if (s_Hints.init.angleType == AnglePlatformType::Vulkan)
                {
                    type = EGL_PLATFORM_ANGLE_TYPE_VULKAN_ANGLE;
                }
            }

            if (type)
            {
                *attribs = (EGLint*)calloc(3, sizeof(EGLint));
                (*attribs)[0] = EGL_PLATFORM_ANGLE_TYPE_ANGLE;
                (*attribs)[1] = type;
                (*attribs)[2] = EGL_NONE;

                return EGL_PLATFORM_ANGLE_ANGLE;
            }
        }

        return 0;
    }

    EGLNativeDisplayType Platform::PlatformGetEglNativeDisplay()
    {
        return GetDC(WindowsPlatform::s_HelperWindowHandle);
    }

    EGLNativeWindowType Platform::PlatformGetEglNativeWindow(Window* window)
    {
        return window->GetNativeHandle();
    }



    void* Platform::OpenLibrary(const std::string& libName)
    {
        return LoadLibraryA(libName.c_str());
    }

    bool Platform::CloseLibrary(void* handle)
    {
        return FreeLibrary((HMODULE)handle);
    }

    void* Platform::GetLibraryProcAddress(void* handle, const std::string& procName)
    {
        return GetProcAddress((HMODULE)handle, procName.c_str());
    }


    bool WindowsPlatform::LoadLibraries()
    {
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
        if (s_Libs.xInput.instance) FreeLibrary(s_Libs.xInput.instance);
        if (s_Libs.dInput8.instance) FreeLibrary(s_Libs.dInput8.instance);
        if (s_Libs.winmm.instance) FreeLibrary(s_Libs.winmm.instance);
        if (s_Libs.user32.instance) FreeLibrary(s_Libs.user32.instance);
        if (s_Libs.dwmapi.instance) FreeLibrary(s_Libs.dwmapi.instance);
        if (s_Libs.shcore.instance) FreeLibrary(s_Libs.shcore.instance);
        if (s_Libs.ntdll.instance) FreeLibrary(s_Libs.ntdll.instance);
        s_Libs = {};
    }


    void WindowsPlatform::CreateKeyTables()
    {
        int32_t scancode;

        memset(s_Keycodes, -1, sizeof(s_Keycodes));
        memset(s_Scancodes, -1, sizeof(s_Scancodes));

        s_Keycodes[0x00B] = Key::NumRow0;
        s_Keycodes[0x002] = Key::NumRow1;
        s_Keycodes[0x003] = Key::NumRow2;
        s_Keycodes[0x004] = Key::NumRow3;
        s_Keycodes[0x005] = Key::NumRow4;
        s_Keycodes[0x006] = Key::NumRow5;
        s_Keycodes[0x007] = Key::NumRow6;
        s_Keycodes[0x008] = Key::NumRow7;
        s_Keycodes[0x009] = Key::NumRow8;
        s_Keycodes[0x00A] = Key::NumRow9;
        s_Keycodes[0x01E] = Key::A;
        s_Keycodes[0x030] = Key::B;
        s_Keycodes[0x02E] = Key::C;
        s_Keycodes[0x020] = Key::D;
        s_Keycodes[0x012] = Key::E;
        s_Keycodes[0x021] = Key::F;
        s_Keycodes[0x022] = Key::G;
        s_Keycodes[0x023] = Key::H;
        s_Keycodes[0x017] = Key::I;
        s_Keycodes[0x024] = Key::J;
        s_Keycodes[0x025] = Key::K;
        s_Keycodes[0x026] = Key::L;
        s_Keycodes[0x032] = Key::M;
        s_Keycodes[0x031] = Key::N;
        s_Keycodes[0x018] = Key::O;
        s_Keycodes[0x019] = Key::P;
        s_Keycodes[0x010] = Key::Q;
        s_Keycodes[0x013] = Key::R;
        s_Keycodes[0x01F] = Key::S;
        s_Keycodes[0x014] = Key::T;
        s_Keycodes[0x016] = Key::U;
        s_Keycodes[0x02F] = Key::V;
        s_Keycodes[0x011] = Key::W;
        s_Keycodes[0x02D] = Key::X;
        s_Keycodes[0x015] = Key::Y;
        s_Keycodes[0x02C] = Key::Z;

        s_Keycodes[0x028] = Key::Apostrophe;
        s_Keycodes[0x02B] = Key::Backslash;
        s_Keycodes[0x033] = Key::Comma;
        s_Keycodes[0x00D] = Key::Equal;
        s_Keycodes[0x029] = Key::GraveAccent;
        s_Keycodes[0x01A] = Key::LeftBracket;
        s_Keycodes[0x00C] = Key::Minus;
        s_Keycodes[0x034] = Key::Period;
        s_Keycodes[0x01B] = Key::RightBracket;
        s_Keycodes[0x027] = Key::Semicolon;
        s_Keycodes[0x035] = Key::Slash;
        s_Keycodes[0x056] = Key::World2;

        s_Keycodes[0x00E] = Key::Backspace;
        s_Keycodes[0x153] = Key::Delete;
        s_Keycodes[0x14F] = Key::End;
        s_Keycodes[0x01C] = Key::Enter;
        s_Keycodes[0x001] = Key::Escape;
        s_Keycodes[0x147] = Key::Home;
        s_Keycodes[0x152] = Key::Insert;
        s_Keycodes[0x15D] = Key::Menu;
        s_Keycodes[0x151] = Key::PageDown;
        s_Keycodes[0x149] = Key::PageUp;
        s_Keycodes[0x045] = Key::Pause;
        s_Keycodes[0x146] = Key::Pause;
        s_Keycodes[0x039] = Key::Space;
        s_Keycodes[0x00F] = Key::Tab;
        s_Keycodes[0x03A] = Key::CapsLock;
        s_Keycodes[0x145] = Key::NumLock;
        s_Keycodes[0x046] = Key::ScrollLock;
        s_Keycodes[0x03B] = Key::F1;
        s_Keycodes[0x03C] = Key::F2;
        s_Keycodes[0x03D] = Key::F3;
        s_Keycodes[0x03E] = Key::F4;
        s_Keycodes[0x03F] = Key::F5;
        s_Keycodes[0x040] = Key::F6;
        s_Keycodes[0x041] = Key::F7;
        s_Keycodes[0x042] = Key::F8;
        s_Keycodes[0x043] = Key::F9;
        s_Keycodes[0x044] = Key::F10;
        s_Keycodes[0x057] = Key::F11;
        s_Keycodes[0x058] = Key::F12;
        s_Keycodes[0x064] = Key::F13;
        s_Keycodes[0x065] = Key::F14;
        s_Keycodes[0x066] = Key::F15;
        s_Keycodes[0x067] = Key::F16;
        s_Keycodes[0x068] = Key::F17;
        s_Keycodes[0x069] = Key::F18;
        s_Keycodes[0x06A] = Key::F19;
        s_Keycodes[0x06B] = Key::F20;
        s_Keycodes[0x06C] = Key::F21;
        s_Keycodes[0x06D] = Key::F22;
        s_Keycodes[0x06E] = Key::F23;
        s_Keycodes[0x076] = Key::F24;
        s_Keycodes[0x038] = Key::LeftAlt;
        s_Keycodes[0x01D] = Key::LeftControl;
        s_Keycodes[0x02A] = Key::LeftShift;
        s_Keycodes[0x15B] = Key::LeftSuper;
        s_Keycodes[0x137] = Key::PrintScreen;
        s_Keycodes[0x138] = Key::RightAlt;
        s_Keycodes[0x11D] = Key::RightControl;
        s_Keycodes[0x036] = Key::RightShift;
        s_Keycodes[0x15C] = Key::RightSuper;
        s_Keycodes[0x150] = Key::Down;
        s_Keycodes[0x14B] = Key::Left;
        s_Keycodes[0x14D] = Key::Right;
        s_Keycodes[0x148] = Key::Up;

        s_Keycodes[0x052] = Key::KeyPad0;
        s_Keycodes[0x04F] = Key::KeyPad1;
        s_Keycodes[0x050] = Key::KeyPad2;
        s_Keycodes[0x051] = Key::KeyPad3;
        s_Keycodes[0x04B] = Key::KeyPad4;
        s_Keycodes[0x04C] = Key::KeyPad5;
        s_Keycodes[0x04D] = Key::KeyPad6;
        s_Keycodes[0x047] = Key::KeyPad7;
        s_Keycodes[0x048] = Key::KeyPad8;
        s_Keycodes[0x049] = Key::KeyPad9;
        s_Keycodes[0x04E] = Key::KeyPadAdd;
        s_Keycodes[0x053] = Key::KeyPadDecimal;
        s_Keycodes[0x135] = Key::KeyPadDivide;
        s_Keycodes[0x11C] = Key::KeyPadEnter;
        s_Keycodes[0x059] = Key::KeyPadEqual;
        s_Keycodes[0x037] = Key::KeyPadMultiply;
        s_Keycodes[0x04A] = Key::KeyPadSubtract;

        for (scancode = 0; scancode < 512; scancode++)
        {
            if ((int32_t)s_Keycodes[scancode] > 0)
            {
                s_Scancodes[(int32_t)s_Keycodes[scancode]] = scancode;
            }
        }
    }

    /// <summary> Updates key names according to the current keyboard layout </summary>
    void WindowsPlatform::UpdateKeyNames()
    {
        memset(s_KeyNames, 0, sizeof(s_KeyNames));

        for (int32_t key = (int32_t)Key::Space; key < (int32_t)Key::Count; key++)
        {
            int32_t scancode = s_Scancodes[key];
            if (scancode == -1)
            {
                continue;
            }

            UINT vk;
            if (key >= (int32_t)Key::KeyPad0
                && key <= (int32_t)Key::KeyPadAdd)
            {
                const UINT vks[] =
                {
                    VK_NUMPAD0,  VK_NUMPAD1,  VK_NUMPAD2, VK_NUMPAD3,
                    VK_NUMPAD4,  VK_NUMPAD5,  VK_NUMPAD6, VK_NUMPAD7,
                    VK_NUMPAD8,  VK_NUMPAD9,  VK_DECIMAL, VK_DIVIDE,
                    VK_MULTIPLY, VK_SUBTRACT, VK_ADD
                };

                vk = vks[key - (int32_t)Key::KeyPad0];
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
                //dead-key character (accent or diacritic)
                //QUESTION: why do we retry this call?
                length = ToUnicode(vk, scancode, state, chars, sizeof(chars) / sizeof(WCHAR), 0);
            }

            if (length < 1)
            {
                continue;
            }

            WideStringToUTF8(chars, s_KeyNames[key], 1);
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

        size = WideCharToMultiByte(CP_UTF8, 0, source, -1, nullptr, 0, NULL, NULL);
        if (!size)
        {
            return nullptr;
        }

        target = (char*)calloc(size, 1);

        if (!WideCharToMultiByte(CP_UTF8, 0, source, -1, target, size, NULL, NULL))
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
        return WideCharToMultiByte(CP_UTF8, 0, source, -1, target, sizeof(target), NULL, NULL);
    }

    bool WindowsPlatform::WideStringToUTF8(const WCHAR source[], char target[], int32_t processCharCount)
    {
        return WideCharToMultiByte(CP_UTF8, 0, source, processCharCount, target, sizeof(target), NULL, NULL);
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
