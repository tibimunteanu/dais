#include "engine/core/Platform.h"

namespace dais
{
    Window* Platform::OpenWindow(const std::string& title, uint32_t width, uint32_t height)
    {
        Window* window = Window::Create(title, width, height);
        m_Windows.push_back(window);
        return window;
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
        std::cout << "[Platform] GetWindows" << std::endl;

        return m_Windows;
    }

    Window* Platform::GetPrimaryWindow() const
    {
        std::cout << "[Platform] GetPrimaryWindow" << std::endl;

        return m_Windows.size() > 0
            ? m_Windows[0]
            : nullptr;
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
