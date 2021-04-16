#pragma once

#include "engine/core/Base.h"
#include "engine/core/Monitor.h"
#include "engine/core/Window.h"

typedef void(*MonitorConnectedCallback)(dais::Monitor*);

namespace dais
{
    class Platform
    {
    protected:
        static std::vector<Window*> s_Windows;
        static std::vector<Monitor*> s_Monitors;

    protected:
        static struct Callbacks
        {
            MonitorConnectedCallback MonitorConnected;
            MonitorConnectedCallback MonitorDisconnected;
        } s_Callbacks;

    public:
        static void Init();
        static void Terminate();

        static Window* OpenWindow(WindowConfig config, FramebufferConfig fbConfig, Monitor* monitor);

        static const std::vector<Window*>& GetWindows();
        static Window* GetPrimaryWindow();

        static const std::vector<Monitor*>& GetMonitors();
        static Monitor* GetPrimaryMonitor();

        static void SetMonitorConnectedCallback(MonitorConnectedCallback callback);
        static void SetMonitorDisconnectedCallback(MonitorConnectedCallback callback);

        static void PollEvents();
        static void WaitEvents();
        static void WaitEventsTimeout(double timeout);
    };
}
