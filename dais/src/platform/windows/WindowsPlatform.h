#pragma once

#include "engine/core/Platform.h"
#include "platform/windows/WindowsBase.h"
#include "platform/windows/WindowsMonitor.h"
#include "platform/windows/WindowsWindow.h"

namespace dais
{
    class WindowsPlatform : public Platform
    {
    public:
        static HWND s_HelperWindowHandle;
        static HDEVNOTIFY s_DeviceNotificationHandle;
        static DWORD s_ForegroundLockTimeout;
        static struct WindowsLibs
        {
            struct XInputLib
            {
                HINSTANCE Instance;
                PFN_XInputGetCapabilities GetCapabilities;
                PFN_XInputGetState GetState;
            } XInput;

            struct DInput8Lib
            {
                HINSTANCE Instance;
                PFN_DirectInput8Create Create;
                IDirectInput8W* API;
            } DInput8;

            struct WinmmLib
            {
                HINSTANCE Instance;
                PFN_timeGetTime GetTime;
            } Winmm;

            struct User32Lib
            {
                HINSTANCE Instance;
                PFN_SetProcessDPIAware SetProcessDPIAware;
                PFN_ChangeWindowMessageFilterEx ChangeWindowMessageFilterEx;
                PFN_EnableNonClientDpiScaling EnableNonClientDpiScaling;
                PFN_SetProcessDpiAwarenessContext SetProcessDpiAwarenessContext;
                PFN_GetDpiForWindow GetDpiForWindow;
                PFN_AdjustWindowRectExForDpi AdjustWindowRectExForDpi;
            } User32;

            struct DwmapiLib
            {
                HINSTANCE Instance;
                PFN_DwmIsCompositionEnabled IsCompositionEnabled;
                PFN_DwmFlush Flush;
                PFN_DwmEnableBlurBehindWindow EnableBlurBehindWindow;
                PFN_DwmGetColorizationColor GetColorizationColor;
            } Dwmapi;

            struct ShcoreLib
            {
                HINSTANCE Instance;
                PFN_SetProcessDpiAwareness SetProcessDpiAwareness;
                PFN_GetDpiForMonitor GetDpiForMonitor;
            } Shcore;

            struct NtdllLib
            {
                HINSTANCE Instance;
                PFN_RtlVerifyVersionInfo RtlVerifyVersionInfo;
            } Ntdll;
        } s_Libs;

    public:
        static void PlatformInit();
        static void PlatformTerminate();

        static bool LoadLibraries();
        static void FreeLibraries();
        static void SetProcessDpiAware();
        static void SetForegroundLockTimeout();
        static void RestoreForegroundLockTimeout();
        static void AdjustRect(RECT* rect, HWND windowHandle, DWORD style, DWORD styleEx);
        static void AdjustRect(RECT* rect, DWORD style, DWORD styleEx, UINT dpi);
        static void PollMonitors();
        static bool RegisterWindowClass();
        static void UnregisterWindowClass();
        static bool CreateHelperWindow();

        static bool IsWindowsVersionOrGreater(WORD major, WORD minor, WORD sp);
        static bool IsWindowsVistaOrGreater();
        static bool IsWindows7OrGreater();
        static bool IsWindows8OrGreater();
        static bool IsWindows8Point1OrGreater();
        static bool IsWindows10BuildOrGreater(WORD build);
        static bool IsWindows10AnniversaryUpdateOrGreater();
        static bool IsWindows10CreatorsUpdateOrGreater();

        static char* WideStringToUTF8(const WCHAR* source);
        static bool WideStringToUTF8(const WCHAR* source, std::string& target);
        static bool WideStringToUTF8(const WCHAR source[], char target[]);
        static WCHAR* UTF8ToWideString(const char* source);

    };
}

