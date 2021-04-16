#pragma once

#include "engine/core/Base.h"
#include "engine/core/Monitor.h"
#include "engine/core/Window.h"

namespace dais
{
    typedef void(*MonitorCallback)(Monitor*);

    class Platform
    {
    protected:
        static std::vector<Window*> s_Windows;
        static std::vector<Monitor*> s_Monitors;

    protected:
        static struct Callbacks
        {
            MonitorCallback MonitorConnected;
            MonitorCallback MonitorDisconnected;
        } s_Callbacks;

    public:
        static void Init();
        static void Terminate();

        static Window* OpenWindow(WindowConfig config, FramebufferConfig fbConfig, Monitor* monitor);

        static const std::vector<Window*>& GetWindows();
        static Window* GetPrimaryWindow();

        static const std::vector<Monitor*>& GetMonitors();
        static Monitor* GetPrimaryMonitor();

        static void SetMonitorConnectedCallback(MonitorCallback callback);
        static void SetMonitorDisconnectedCallback(MonitorCallback callback);

        static void PollEvents();
        static void WaitEvents();
        static void WaitEventsTimeout(double timeout);
    };
}
