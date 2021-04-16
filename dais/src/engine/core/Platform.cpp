#include "engine/core/Platform.h"

namespace dais
{
    ////////////////////////////////////// STATIC MEMBERS /////////////////////////////////////////

    std::vector<Window*> Platform::s_Windows = {};
    std::vector<Monitor*> Platform::s_Monitors = {};
    Platform::Callbacks Platform::s_Callbacks = {};



    //////////////////////////////////////// STATIC API ///////////////////////////////////////////

    Window* Platform::OpenWindow(WindowConfig config, FramebufferConfig fbConfig, Monitor* monitor)
    {
        Window* window = Window::Create(config, fbConfig, monitor);
        s_Windows.push_back(window);
        return window;
    }

    const std::vector<Window*>& Platform::GetWindows()
    {
        DAIS_TRACE("[Platform] GetWindows");

        return s_Windows;
    }

    Window* Platform::GetPrimaryWindow()
    {
        DAIS_TRACE("[Platform] GetPrimaryWindow");

        return s_Windows.size() > 0
            ? s_Windows[0]
            : nullptr;
    }

    const std::vector<Monitor*>& Platform::GetMonitors()
    {
        DAIS_TRACE("[Platform] GetMonitors");

        return s_Monitors;
    }

    Monitor* Platform::GetPrimaryMonitor()
    {
        DAIS_TRACE("[Platform] GetPrimaryMonitor");

        return s_Monitors.size() > 0
            ? s_Monitors[0]
            : nullptr;
    }

    void Platform::SetMonitorConnectedCallback(MonitorConnectedCallback callback)
    {
        s_Callbacks.MonitorConnected = callback;
    }

    void Platform::SetMonitorDisconnectedCallback(MonitorConnectedCallback callback)
    {
        s_Callbacks.MonitorDisconnected = callback;
    }
}
