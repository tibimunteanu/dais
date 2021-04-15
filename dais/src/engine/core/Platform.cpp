#include "engine/core/Platform.h"

namespace dais
{
    Window* Platform::OpenWindow(WindowConfig config, FramebufferConfig fbConfig, Monitor* monitor) {
        Window* window = Window::Create(config, fbConfig, monitor);
        m_Windows.push_back(window);
        return window;
    }

    Platform::Platform()
    {
        DAIS_TRACE("[Platform] Constructor");
    }

    Platform::~Platform()
    {
        DAIS_TRACE("[Platform] Destructor");

        if (m_Monitors.size() > 0)
        {
            for (size_t i = 0; i < m_Monitors.size(); i++)
            {
                //TODO: m_Monitors[i]->RestoreOriginalGammaRamp();
                delete m_Monitors[i];
            }
        }

        if (m_Windows.size() > 0)
        {
            for (size_t i = 0; i < m_Windows.size(); i++)
            {
                delete m_Windows[i];
            }
        }
    }

    const std::vector<Window*>& Platform::GetWindows() const
    {
        DAIS_TRACE("[Platform] GetWindows");

        return m_Windows;
    }

    Window* Platform::GetPrimaryWindow() const
    {
        DAIS_TRACE("[Platform] GetPrimaryWindow");

        return m_Windows.size() > 0
            ? m_Windows[0]
            : nullptr;
    }

    const std::vector<Monitor*>& Platform::GetMonitors() const
    {
        DAIS_TRACE("[Platform] GetMonitors");

        return m_Monitors;
    }

    Monitor* Platform::GetPrimaryMonitor() const
    {
        DAIS_TRACE("[Platform] GetPrimaryMonitor");

        return m_Monitors.size() > 0
            ? m_Monitors[0]
            : nullptr;
    }

    void Platform::SetMonitorConnectedCallback(MonitorConnectedCallback callback)
    {
        m_Callbacks.MonitorConnected = callback;
    }

    void Platform::SetMonitorDisconnectedCallback(MonitorConnectedCallback callback)
    {
        m_Callbacks.MonitorDisconnected = callback;
    }
}
