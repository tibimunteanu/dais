#pragma once

#include "engine/core/Base.h"

// This is taken from glfw source

// We don't need all the fancy stuff
#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Unicode only
#ifndef UNICODE
#define UNICODE
#endif

// Required Windows XP or later
#if WINVER < 0x0501
#undef WINVER
#define WINVER 0x0501
#endif
#if _WIN32_WINNT < 0x0501
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

// Use DirectInput8 interfaces
#define DIRECTINPUT_VERSION 0x0800

// Use OEM cursor resources
#define OEMRESOURCE

#include <wctype.h>
#include <windows.h>
#include <dinput.h>
#include <xinput.h>
#include <dbt.h>

// HACK: Define macros that some windows.h variants don't
#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL 0x020E
#endif
#ifndef WM_DWMCOMPOSITIONCHANGED
#define WM_DWMCOMPOSITIONCHANGED 0x031E
#endif
#ifndef WM_DWMCOLORIZATIONCOLORCHANGED
#define WM_DWMCOLORIZATIONCOLORCHANGED 0x0320
#endif
#ifndef WM_COPYGLOBALDATA
#define WM_COPYGLOBALDATA 0x0049
#endif
#ifndef WM_UNICHAR
#define WM_UNICHAR 0x0109
#endif
#ifndef UNICODE_NOCHAR
#define UNICODE_NOCHAR 0xFFFF
#endif
#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif
#ifndef GET_XBUTTON_WPARAM
#define GET_XBUTTON_WPARAM(w) (HIWORD(w))
#endif
#ifndef EDS_ROTATEDMODE
#define EDS_ROTATEDMODE 0x00000004
#endif
#ifndef DISPLAY_DEVICE_ACTIVE
#define DISPLAY_DEVICE_ACTIVE 0x00000001
#endif
#ifndef _WIN32_WINNT_WINBLUE
#define _WIN32_WINNT_WINBLUE 0x0603
#endif
#ifndef _WIN32_WINNT_WIN8
#define _WIN32_WINNT_WIN8 0x0602
#endif
#ifndef WM_GETDPISCALEDSIZE
#define WM_GETDPISCALEDSIZE 0x02e4
#endif
#ifndef USER_DEFAULT_SCREEN_DPI
#define USER_DEFAULT_SCREEN_DPI 96
#endif
#ifndef OCR_HAND
#define OCR_HAND 32649
#endif

#if WINVER < 0x0601
typedef struct
{
    DWORD cbSize;
    DWORD ExtStatus;
} CHANGEFILTERSTRUCT;
#ifndef MSGFLT_ALLOW
#define MSGFLT_ALLOW 1
#endif
#endif /*Windows 7*/

#if WINVER < 0x0600
#define DWM_BB_ENABLE 0x00000001
#define DWM_BB_BLURREGION 0x00000002
typedef struct
{
    DWORD dwFlags;
    BOOL fEnable;
    HRGN hRgnBlur;
    BOOL fTransitionOnMaximized;
} DWM_BLURBEHIND;
#else
#include <dwmapi.h>
#endif /*Windows Vista*/

#ifndef DPI_ENUMS_DECLARED
typedef enum
{
    PROCESS_DPI_UNAWARE = 0,
    PROCESS_SYSTEM_DPI_AWARE = 1,
    PROCESS_PER_MONITOR_DPI_AWARE = 2
} PROCESS_DPI_AWARENESS;
typedef enum
{
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;
#endif /*DPI_ENUMS_DECLARED*/

#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((HANDLE) -4)
#endif /*DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2*/

// HACK: Define macros that some xinput.h variants don't
#ifndef XINPUT_CAPS_WIRELESS
#define XINPUT_CAPS_WIRELESS 0x0002
#endif
#ifndef XINPUT_DEVSUBTYPE_WHEEL
#define XINPUT_DEVSUBTYPE_WHEEL 0x02
#endif
#ifndef XINPUT_DEVSUBTYPE_ARCADE_STICK
#define XINPUT_DEVSUBTYPE_ARCADE_STICK 0x03
#endif
#ifndef XINPUT_DEVSUBTYPE_FLIGHT_STICK
#define XINPUT_DEVSUBTYPE_FLIGHT_STICK 0x04
#endif
#ifndef XINPUT_DEVSUBTYPE_DANCE_PAD
#define XINPUT_DEVSUBTYPE_DANCE_PAD 0x05
#endif
#ifndef XINPUT_DEVSUBTYPE_GUITAR
#define XINPUT_DEVSUBTYPE_GUITAR 0x06
#endif
#ifndef XINPUT_DEVSUBTYPE_DRUM_KIT
#define XINPUT_DEVSUBTYPE_DRUM_KIT 0x08
#endif
#ifndef XINPUT_DEVSUBTYPE_ARCADE_PAD
#define XINPUT_DEVSUBTYPE_ARCADE_PAD 0x13
#endif
#ifndef XUSER_MAX_COUNT
#define XUSER_MAX_COUNT 4
#endif

// HACK: Define macros that some dinput.h variants don't
#ifndef DIDFT_OPTIONAL
#define DIDFT_OPTIONAL 0x80000000
#endif

// xinput.dll function pointer typedefs
typedef DWORD(WINAPI* PFN_XInputGetCapabilities)(DWORD, DWORD, XINPUT_CAPABILITIES*);
typedef DWORD(WINAPI* PFN_XInputGetState)(DWORD, XINPUT_STATE*);

// dinput8.dll function pointer typedefs
typedef HRESULT(WINAPI* PFN_DirectInput8Create)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);

// winmm.dll function pointer typedefs
typedef DWORD(WINAPI* PFN_timeGetTime)(void);

// user32.dll function pointer typedefs
typedef BOOL(WINAPI* PFN_SetProcessDPIAware)(void);
typedef BOOL(WINAPI* PFN_ChangeWindowMessageFilterEx)(HWND, UINT, DWORD, CHANGEFILTERSTRUCT*);
typedef BOOL(WINAPI* PFN_EnableNonClientDpiScaling)(HWND);
typedef BOOL(WINAPI* PFN_SetProcessDpiAwarenessContext)(HANDLE);
typedef UINT(WINAPI* PFN_GetDpiForWindow)(HWND);
typedef BOOL(WINAPI* PFN_AdjustWindowRectExForDpi)(LPRECT, DWORD, BOOL, DWORD, UINT);

// dwmapi.dll function pointer typedefs
typedef HRESULT(WINAPI* PFN_DwmIsCompositionEnabled)(BOOL*);
typedef HRESULT(WINAPI* PFN_DwmFlush)(VOID);
typedef HRESULT(WINAPI* PFN_DwmEnableBlurBehindWindow)(HWND, const DWM_BLURBEHIND*);
typedef HRESULT(WINAPI* PFN_DwmGetColorizationColor)(DWORD*, BOOL*);

// shcore.dll function pointer typedefs
typedef HRESULT(WINAPI* PFN_SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);
typedef HRESULT(WINAPI* PFN_GetDpiForMonitor)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);

// ntdll.dll function pointer typedefs
typedef LONG(WINAPI* PFN_RtlVerifyVersionInfo)(OSVERSIONINFOEXW*, ULONG, ULONGLONG);

namespace dais
{
    class WindowsBase
    {
    public:
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
        } Libs;

    private:
        WindowsBase() {}

    public:
        static bool LoadLibraries();
        static void FreeLibraries();
        static void SetProcessDpiAware();

        static bool IsWindowsVersionOrGreater(WORD major, WORD minor, WORD sp);
        static bool IsWindowsVistaOrGreater();
        static bool IsWindows7OrGreater();
        static bool IsWindows8OrGreater();
        static bool IsWindows8Point1OrGreater();
        static bool IsWindows10BuildOrGreater(WORD build);
        static bool IsWindows10AnniversaryUpdateOrGreater();
        static bool IsWindows10CreatorsUpdateOrGreater();

        static char* WideStringToUTF8(const WCHAR* source);
        static bool WindowsBase::WideStringToUTF8(const WCHAR* source, std::string& target);
        static bool WindowsBase::WideStringToUTF8(const WCHAR source[], char target[]);
        static WCHAR* UTF8ToWideString(const char* source);
    };
}
