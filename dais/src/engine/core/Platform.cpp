#include "engine/core/Platform.h"

namespace dais
{
    Window* Platform::OpenWindow(const std::string& title, uint32_t width, uint32_t height)
    {
        m_Window = Window::Create(title, width, height);
        return m_Window;
    }

    Platform::Platform()
    {
        std::cout << "[Platform] Constructor" << std::endl;
    }

    Platform::~Platform()
    {
        std::cout << "[Platform] Destructor" << std::endl;

        if (m_Monitors.size() > 0)
        {
            for (size_t i = 0; i < m_Monitors.size(); i++)
            {
                //TODO: m_Monitors[i]->RestoreOriginalGammaRamp();
                delete m_Monitors[i];
            }
        }

        if (m_Window)
        {
            delete m_Window;
        }
    }

    Window* Platform::GetPrimaryWindow() const
    {
        return m_Window;
    }

    const std::vector<Monitor*>& Platform::GetMonitors() const
    {
        std::cout << "[Platform] GetMonitors" << std::endl;

        return m_Monitors;
    }

    Monitor* Platform::GetPrimaryMonitor() const
    {
        std::cout << "[Platform] GetPrimaryMonitor" << std::endl;

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
