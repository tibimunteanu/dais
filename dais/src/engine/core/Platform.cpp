#include "engine/core/Platform.h"

namespace dais
{
    ////////////////////////////////////// STATIC MEMBERS /////////////////////////////////////////

    std::vector<Window*> Platform::s_Windows = {};
    std::vector<Monitor*> Platform::s_Monitors = {};
    std::vector<Cursor*> Platform::s_Cursors = {};
    Platform::Callbacks Platform::s_Callbacks = {};



    //////////////////////////////////////// STATIC API ///////////////////////////////////////////

    bool Platform::Init()
    {
        return Platform::PlatformInit();
    }

    void Platform::Terminate()
    {
        if (s_Monitors.size() > 0)
        {
            for (size_t i = 0; i < s_Monitors.size(); i++)
            {
                s_Monitors[i]->RestoreOriginalGammaRamp();
                delete s_Monitors[i];
            }
        }

        if (s_Windows.size() > 0)
        {
            for (size_t i = 0; i < s_Windows.size(); i++)
            {
                delete s_Windows[i];
            }
        }

        Platform::PlatformTerminate();
    }

    void Platform::PollEvents()
    {
        Platform::PlatformPollEvents();
    }

    void Platform::WaitEvents()
    {
        Platform::PlatformWaitEvents();
    }

    void Platform::WaitEventsTimeout(double timeout)
    {
        Platform::PlatformWaitEventsTimeout(timeout);
    }


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

    void Platform::SetMonitorConnectedCallback(MonitorCallback callback)
    {
        s_Callbacks.monitorConnected = callback;
    }

    void Platform::SetMonitorDisconnectedCallback(MonitorCallback callback)
    {
        s_Callbacks.monitorDisconnected = callback;
    }
}
