#pragma once

#include "engine/core/Base.h"
#include "engine/core/Cursor.h"
#include "engine/core/Monitor.h"
#include "engine/core/Window.h"

#define DAIS_CURSOR_NORMAL          1
#define DAIS_CURSOR_HIDDEN          2
#define DAIS_CURSOR_DISABLED        3

#define DAIS_MOD_SHIFT           0x0001
#define DAIS_MOD_CONTROL         0x0002
#define DAIS_MOD_ALT             0x0004
#define DAIS_MOD_SUPER           0x0008
#define DAIS_MOD_CAPS_LOCK       0x0010
#define DAIS_MOD_NUM_LOCK        0x0020

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
    };
}
