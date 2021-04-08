#include "platform/windows/WindowsBase.h"

namespace dais
{
    WindowsBase::WindowsLibs WindowsBase::Libs = WindowsBase::WindowsLibs{};


    bool WindowsBase::LoadLibraries()
    {
        std::cout << "[WindowsBase] Loading libraries" << std::endl;

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
                Libs.XInput.Instance = LoadLibraryA(names[i]);
                if (Libs.XInput.Instance)
                {
                    Libs.XInput.GetCapabilities = (PFN_XInputGetCapabilities)GetProcAddress(Libs.XInput.Instance, "XInputGetCapabilities");
                    Libs.XInput.GetState = (PFN_XInputGetState)GetProcAddress(Libs.XInput.Instance, "XInputGetState");
                    break;
                }
            }
        }

        Libs.DInput8.Instance = LoadLibraryA("dinput8.dll");
        if (Libs.DInput8.Instance)
        {
            Libs.DInput8.Create = (PFN_DirectInput8Create)GetProcAddress(Libs.DInput8.Instance, "DirectInput8Create");
        }

        Libs.Winmm.Instance = LoadLibraryA("winmm.dll");
        if (!Libs.Winmm.Instance)
        {
            throw std::runtime_error("[WindowsBase] LoadLibraries(): Failed to load winmm.dll!");
            return false;
        }

        Libs.Winmm.GetTime = (PFN_timeGetTime)GetProcAddress(Libs.Winmm.Instance, "timeGetTime");

        Libs.User32.Instance = LoadLibraryA("user32.dll");
        if (!Libs.User32.Instance)
        {
            throw std::runtime_error("[WindowsBase] LoadLibraries(): Failed to load user32.dll!");
            return false;
        }

        Libs.User32.SetProcessDPIAware = (PFN_SetProcessDPIAware)GetProcAddress(Libs.User32.Instance, "SetProcessDPIAware");
        Libs.User32.ChangeWindowMessageFilterEx = (PFN_ChangeWindowMessageFilterEx)GetProcAddress(Libs.User32.Instance, "ChangeWindowMessageFilterEx");
        Libs.User32.EnableNonClientDpiScaling = (PFN_EnableNonClientDpiScaling)GetProcAddress(Libs.User32.Instance, "EnableNonClientDpiScaling");
        Libs.User32.SetProcessDpiAwarenessContext = (PFN_SetProcessDpiAwarenessContext)GetProcAddress(Libs.User32.Instance, "SetProcessDpiAwarenessContext");
        Libs.User32.GetDpiForWindow = (PFN_GetDpiForWindow)GetProcAddress(Libs.User32.Instance, "GetDpiForWindow");
        Libs.User32.AdjustWindowRectExForDpi = (PFN_AdjustWindowRectExForDpi)GetProcAddress(Libs.User32.Instance, "AdjustWindowRectExForDpi");

        Libs.Dwmapi.Instance = LoadLibraryA("dwmapi.dll");
        if (Libs.Dwmapi.Instance)
        {
            Libs.Dwmapi.IsCompositionEnabled = (PFN_DwmIsCompositionEnabled)GetProcAddress(Libs.Dwmapi.Instance, "DwmIsCompositionEnabled");
            Libs.Dwmapi.Flush = (PFN_DwmFlush)GetProcAddress(Libs.Dwmapi.Instance, "DwmFlush");
            Libs.Dwmapi.EnableBlurBehindWindow = (PFN_DwmEnableBlurBehindWindow)GetProcAddress(Libs.Dwmapi.Instance, "DwmEnableBlurBehindWindow");
            Libs.Dwmapi.GetColorizationColor = (PFN_DwmGetColorizationColor)GetProcAddress(Libs.Dwmapi.Instance, "DwmGetColorizationColor");
        }

        Libs.Shcore.Instance = LoadLibraryA("shcore.dll");
        if (Libs.Shcore.Instance)
        {
            Libs.Shcore.SetProcessDpiAwareness = (PFN_SetProcessDpiAwareness)GetProcAddress(Libs.Shcore.Instance, "SetProcessDpiAwareness");
            Libs.Shcore.GetDpiForMonitor = (PFN_GetDpiForMonitor)GetProcAddress(Libs.Shcore.Instance, "GetDpiForMonitor");
        }

        Libs.Ntdll.Instance = LoadLibraryA("ntdll.dll");
        if (Libs.Ntdll.Instance)
        {
            Libs.Ntdll.RtlVerifyVersionInfo = (PFN_RtlVerifyVersionInfo)GetProcAddress(Libs.Ntdll.Instance, "RtlVerifyVersionInfo");
        }

        return true;
    }

    void WindowsBase::FreeLibraries()
    {
        std::cout << "[WindowsBase] Free libraries" << std::endl;

        if (Libs.XInput.Instance) FreeLibrary(Libs.XInput.Instance);
        if (Libs.DInput8.Instance) FreeLibrary(Libs.DInput8.Instance);
        if (Libs.Winmm.Instance) FreeLibrary(Libs.Winmm.Instance);
        if (Libs.User32.Instance) FreeLibrary(Libs.User32.Instance);
        if (Libs.Dwmapi.Instance) FreeLibrary(Libs.Dwmapi.Instance);
        if (Libs.Shcore.Instance) FreeLibrary(Libs.Shcore.Instance);
        if (Libs.Ntdll.Instance) FreeLibrary(Libs.Ntdll.Instance);
    }


    bool WindowsBase::IsWindowsVersionOrGreater(WORD major, WORD minor, WORD sp)
    {
        OSVERSIONINFOEXW osvi = { sizeof(osvi), major, minor, 0, 0, {0}, sp };
        DWORD mask = VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR;
        ULONGLONG cond = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
        cond = VerSetConditionMask(cond, VER_MINORVERSION, VER_GREATER_EQUAL);
        cond = VerSetConditionMask(cond, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
        // HACK: Use RtlVerifyVersionInfo instead of VerifyVersionInfoW as the
        //       latter lies unless the user knew to embed a non-default manifest
        //       announcing support for Windows 10 via supportedOS GUID
        return WindowsBase::Libs.Ntdll.RtlVerifyVersionInfo(&osvi, mask, cond) == 0;
    }

    bool WindowsBase::IsWindowsVistaOrGreater()
    {
        return WindowsBase::IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 0);
    }

    bool WindowsBase::IsWindows7OrGreater()
    {
        return WindowsBase::IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 0);
    }

    bool WindowsBase::IsWindows8OrGreater()
    {
        return WindowsBase::IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN8), LOBYTE(_WIN32_WINNT_WIN8), 0);
    }

    bool WindowsBase::IsWindows8Point1OrGreater()
    {
        return WindowsBase::IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINBLUE), LOBYTE(_WIN32_WINNT_WINBLUE), 0);
    }

    bool WindowsBase::IsWindows10AnniversaryUpdateOrGreater()
    {
        return WindowsBase::IsWindows10BuildOrGreater(14393);
    }

    bool WindowsBase::IsWindows10CreatorsUpdateOrGreater()
    {
        return WindowsBase::IsWindows10BuildOrGreater(15063);
    }

    bool WindowsBase::IsWindows10BuildOrGreater(WORD build)
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
        return WindowsBase::Libs.Ntdll.RtlVerifyVersionInfo(&osvi, mask, cond) == 0;
    }


    char* WindowsBase::WideStringToUTF8(const WCHAR* source)
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

    bool WindowsBase::WideStringToUTF8(const WCHAR* source, std::string& target)
    {
        char* name = WindowsBase::WideStringToUTF8(source);
        if (name)
        {
            target = std::string(name);
            free(name);
            return true;
        }
        return false;
    }

    bool WindowsBase::WideStringToUTF8(const WCHAR source[], char target[])
    {
        return WideCharToMultiByte(CP_UTF8, 0, source, -1, target, sizeof(target), nullptr, nullptr);
    }

    WCHAR* WindowsBase::UTF8ToWideString(const char* source)
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