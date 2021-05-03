#pragma once

#include "engine/core/Base.h"
#include "engine/core/ThreadLocalStorage.h"
#include "engine/core/Context.h"
#include "engine/core/EglContext.h"
#include "engine/core/Input.h"
#include "engine/core/Cursor.h"
#include "engine/core/Monitor.h"
#include "engine/core/Window.h"

namespace dais
{
    typedef void(*MonitorCallback)(Monitor*);
    typedef void(*JoystickCallback)(int32_t, int32_t);

    struct InitConfig
    {
        AnglePlatformType angleType;
    };

    class Platform
    {
    public:
        //TODO: why are we persisting these hints?
        static struct Hints
        {
            InitConfig init;
            FramebufferConfig framebuffer;
            WindowConfig window;
            ContextConfig context;
            int32_t refreshRate;
        } s_Hints;

        static uint64_t s_TimerOffset;
        static ThreadLocalStorage* s_ContextSlot;

    protected:
        static std::vector<Window*> s_Windows;
        static std::vector<Monitor*> s_Monitors;
        static std::vector<Cursor*> s_Cursors;

    protected:
        static struct Callbacks
        {
            MonitorCallback monitorConnected;
            MonitorCallback monitorDisconnected;
            JoystickCallback joystickConnected;
            JoystickCallback joystickDisconnected;
        } s_Callbacks;

    public: DAIS_PUBLIC_API
        static bool Init();
        static void Terminate();

        static Window* OpenWindow(const std::string& title, int32_t width, int32_t height, Monitor* monitor);
        static Window* OpenWindow(const std::string& title, int32_t width, int32_t height, 
                                  const WindowConfig* windowConfig, const ContextConfig* contextConfig, const FramebufferConfig* framebufferConfig, Monitor* monitor);

        static const std::vector<Window*>& GetWindows();
        static Window* GetPrimaryWindow();

        static const std::vector<Monitor*>& GetMonitors();
        static Monitor* GetPrimaryMonitor();

        static double GetTime();
        static void SetTime(double time);
        static uint64_t GetTimerValue();
        static uint64_t GetTimerFrequency();

        static bool IsRawMouseMotionSupported();
        static const char* GetKeyName(Key key, int32_t scancode);
        static int32_t GetKeyScancode(Key key);

        static const std::vector<std::string>& GetEglLibNames();
        static const std::vector<std::string>& GetGLES1LibNames();
        static const std::vector<std::string>& GetGLES2LibNames();
        static const std::vector<std::string>& GetGLSLibNames();

        static EGLenum GetEglPlatform(EGLint** attribs);
        static EGLNativeDisplayType GetEglNativeDisplay();
        static EGLNativeWindowType GetEglNativeWindow(Window* window);

        static void* OpenLibrary(const std::string& libName);
        static bool CloseLibrary(void* handle);
        static void* GetLibraryProcAddress(void* handle, const std::string& procName);

        static Cursor* CreateCursor(const Image* image, int32_t xHot, int32_t yHot);
        static Cursor* CreateStandardCursor(CursorShape shape);
        static void DestroyCursor(Cursor* cursor);

        static const char* GetClipboardString();
        static void SetClipboardString(const char* string);

        static void SetMonitorConnectedCallback(MonitorCallback callback);
        static void SetMonitorDisconnectedCallback(MonitorCallback callback);
        static void SetJoystickConnectedCallback(JoystickCallback callback);
        static void SetJoystickDisconnectedCallback(JoystickCallback callback);

        static void PollEvents();
        static void WaitEvents();
        static void WaitEventsTimeout(double timeout);

        static void SetHintsToDefult();

    private: DAIS_PLATFORM_API
        static bool PlatformInit();
        static void PlatformTerminate();

        static void PlatformPollEvents();
        static void PlatformWaitEvents();
        static void PlatformWaitEventsTimeout(double timeout);

        static bool PlatformIsRawMouseMotionSupported();

        static uint64_t PlatformGetTimerValue();
        static uint64_t PlatformGetTimerFrequency();

        static const char* PlatformGetClipboardString();
        static void PlatformSetClipboardString(const char* string);

        static const char* PlatformGetScancodeName(int32_t scancode);
        static int32_t PlatformGetKeyScancode(Key key);

        static const std::vector<std::string>& PlatformGetEglLibNames();
        static const std::vector<std::string>& PlatformGetGLES1LibNames();
        static const std::vector<std::string>& PlatformGetGLES2LibNames();
        static const std::vector<std::string>& PlatformGetGLSLibNames();

        static EGLenum PlatformGetEglPlatform(EGLint** attribs);
        static EGLNativeDisplayType PlatformGetEglNativeDisplay();
        static EGLNativeWindowType PlatformGetEglNativeWindow(Window* window);
    };
}
