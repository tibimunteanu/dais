#include "platform/windows/WindowsPlatform.h"

namespace dais
{
    Platform* Platform::Create(Callbacks callbacks)
    {
        return new WindowsPlatform(callbacks);
    }

    WindowsPlatform::WindowsPlatform(Callbacks callbacks)
        : Platform(callbacks)
    {
        std::cout << "[WindowsPlatform] Constructor" << std::endl;

        WindowsBase::LoadLibraries();

        PollMonitors();
    }

    WindowsPlatform::~WindowsPlatform()
    {
        std::cout << "[WindowsPlatform] Destructor" << std::endl;

        WindowsBase::FreeLibraries();
    }


    void WindowsPlatform::PollMonitors()
    {
        std::cout << "[WindowsPlatform] PollMonitors" << std::endl;

        std::vector<Monitor*> disconnected = m_Monitors;

        uint32_t adapterIndex;
        for (adapterIndex = 0; ; adapterIndex++)
        {
            bool insertFirst = false;

            DISPLAY_DEVICEW adapter = { sizeof(adapter) };

            if (!EnumDisplayDevicesW(nullptr, adapterIndex, &adapter, 0))
            {
                break;
            }

            if (!(adapter.StateFlags & DISPLAY_DEVICE_ACTIVE))
            {
                continue;
            }

            if (adapter.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
            {
                insertFirst = true;
            }

            uint32_t displayIndex;
            for (displayIndex = 0; ; displayIndex++)
            {
                DISPLAY_DEVICEW display = { sizeof(display) };

                if (!EnumDisplayDevicesW(adapter.DeviceName, displayIndex, &display, 0))
                {
                    break;
                }

                if (!(display.StateFlags & DISPLAY_DEVICE_ACTIVE))
                {
                    continue;
                }

                uint32_t d;
                for (d = 0; d < disconnected.size(); d++)
                {
                    if (disconnected[d]
                        && wcscmp(((WindowsMonitor*)disconnected[d])->m_DisplayName, display.DeviceName) == 0)
                    {
                        disconnected[d] = nullptr;

                        EnumDisplayMonitors(nullptr, nullptr, WindowsMonitor::SetHandle, (LPARAM)m_Monitors[d]);
                        break;
                    }
                }

                if (d < disconnected.size())
                {
                    continue;
                }

                WindowsMonitor* monitor = new WindowsMonitor(&adapter, &display);
                if (!monitor)
                {
                    return;
                }

                if (insertFirst)
                {
                    m_Monitors.insert(m_Monitors.begin(), monitor);
                }
                else
                {
                    m_Monitors.push_back(monitor);
                }
                insertFirst = false;

                if (m_Callbacks.MonitorConnected)
                {
                    m_Callbacks.MonitorConnected((Monitor*)monitor);
                }
            }

            if (displayIndex == 0)
            {
                //NOTE: an active adapter could not have any display devices
                //      just add it as a monitor
                uint32_t d;
                for (d = 0; d < disconnected.size(); d++)
                {
                    if (disconnected[d]
                        && wcscmp(((WindowsMonitor*)disconnected[d])->m_DisplayName, adapter.DeviceName) == 0)
                    {
                        disconnected[d] = nullptr;
                        break;
                    }
                }

                if (d < disconnected.size())
                {
                    continue;
                }

                WindowsMonitor* monitor = new WindowsMonitor(&adapter, nullptr);
                if (!monitor)
                {
                    return;
                }

                if (insertFirst)
                {
                    m_Monitors.insert(m_Monitors.begin(), monitor);
                }
                else
                {
                    m_Monitors.push_back(monitor);
                }

                if (m_Callbacks.MonitorConnected)
                {
                    m_Callbacks.MonitorConnected((Monitor*)monitor);
                }
            }
        }

        for (uint32_t d = 0; d < disconnected.size(); d++)
        {
            if (disconnected[d])
            {
                Monitor* monitor = m_Monitors[d];

                m_Monitors.erase(m_Monitors.begin() + d);
                disconnected.erase(disconnected.begin() + d);

                if (m_Callbacks.MonitorDisconnected)
                {
                    m_Callbacks.MonitorDisconnected((Monitor*)monitor);
                }

                delete monitor;
            }
        }
    }
}
