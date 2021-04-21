#pragma once

#include "engine/core/Base.h"
#include "engine/core/Input.h"
#include "engine/core/Cursor.h"
#include "engine/core/Monitor.h"
#include "engine/core/Window.h"

namespace dais
{
    typedef void(*MonitorCallback)(Monitor*);
    typedef void(*JoystickCallback)(int32_t, int32_t);

    class Platform
    {
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

    public:
        static bool Init();
        static void Terminate();

        static Window* OpenWindow(WindowConfig config, FramebufferConfig fbConfig, Monitor* monitor);

        static const std::vector<Window*>& GetWindows();
        static Window* GetPrimaryWindow();

        static const std::vector<Monitor*>& GetMonitors();
        static Monitor* GetPrimaryMonitor();

        static bool IsRawMouseMotionSupported();
        static const char* GetKeyName(int32_t key, int32_t scancode);
        static int32_t GetKeyScancode(int32_t key);

        static Cursor* CreateCursor(const Image* image, int32_t xHot, int32_t yHot);
        static Cursor* CreateStandardCursor(int32_t shape);

        static const char* GetClipboardString();
        static void SetClipboardString(const char* string);

        static void SetMonitorConnectedCallback(MonitorCallback callback);
        static void SetMonitorDisconnectedCallback(MonitorCallback callback);
        static void SetJoystickConnectedCallback(JoystickCallback callback);
        static void SetJoystickDisconnectedCallback(JoystickCallback callback);

        static void PollEvents();
        static void WaitEvents();
        static void WaitEventsTimeout(double timeout);

    private:
        static bool PlatformInit();
        static void PlatformTerminate();
        static void PlatformPollEvents();
        static void PlatformWaitEvents();
        static void PlatformWaitEventsTimeout(double timeout);
        static bool PlatformIsRawMouseMotionSupported();
        static const char* PlatformGetScancodeName(int32_t scancode);
        static int32_t PlatformGetKeyScancode(int32_t key);
    };
}
