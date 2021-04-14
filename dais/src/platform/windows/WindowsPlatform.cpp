#include "platform/windows/WindowsPlatform.h"

namespace dais
{
    Platform* Platform::Create()
    {
        return new WindowsPlatform();
    }

    WindowsPlatform::WindowsPlatform()
    {
        DAIS_TRACE("[WindowsPlatform] Constructor");
    }

    WindowsPlatform::~WindowsPlatform()
    {
        DAIS_TRACE("[WindowsPlatform] Destructor");

        if (m_DeviceNotificationHandle)
        {
            UnregisterDeviceNotification(m_DeviceNotificationHandle);
        }

        if (m_HelperWindowHandle)
        {
            DestroyWindow(m_HelperWindowHandle);
        }

        UnregisterWindowClass();
        RestoreForegroundLockTimeout();

        WindowsBase::FreeLibraries();
    }

    void WindowsPlatform::Init()
    {
        SetForegroundLockTimeout();

        WindowsBase::LoadLibraries();
        WindowsBase::SetProcessDpiAware();

        RegisterWindowClass();
        CreateHelperWindow();
        PollMonitors();
    }

    void WindowsPlatform::PollEvents()
    {
        MSG msg;
        HWND handle;
        Window* window;

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                //this message can only be posted from outside so treat it like a full close
                //TODO: Platform::Terminate
                DAIS_INFO("WM_QUIT");
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }

    void WindowsPlatform::WaitEvents()
    {
        WaitMessage();
        PollEvents();
    }

    void WindowsPlatform::WaitEventsTimeout(double timeout)
    {
        if (timeout != timeout
            || timeout < 0.0
            || timeout > DBL_MAX)
        {
            DAIS_ERROR("Invalid time %f", timeout);
            return;
        }

        MsgWaitForMultipleObjects(0, NULL, FALSE, (DWORD)(timeout * 1000.0), QS_ALLEVENTS);
        PollEvents();
    }


    void WindowsPlatform::SetForegroundLockTimeout()
    {
        SystemParametersInfoW(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &m_ForegroundLockTimeout, 0);
        SystemParametersInfoW(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, UIntToPtr(0), SPIF_SENDCHANGE);
    }

    void WindowsPlatform::RestoreForegroundLockTimeout()
    {
        SystemParametersInfoW(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, UIntToPtr(m_ForegroundLockTimeout), SPIF_SENDCHANGE);
    }

    void WindowsPlatform::PollMonitors()
    {
        DAIS_TRACE("[WindowsPlatform] PollMonitors");

        //copy the array of pointers to the monitors
        std::vector<Monitor*> disconnected = m_Monitors;

        //loop display adapters
        uint32_t adapterIndex;
        for (adapterIndex = 0; ; adapterIndex++)
        {
            bool insertFirst = false;

            DISPLAY_DEVICEW adapter = {};
            adapter.cb = sizeof(adapter);

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

            //loop adapter monitors
            uint32_t displayIndex;
            for (displayIndex = 0; ; displayIndex++)
            {
                DISPLAY_DEVICEW display = {};
                display.cb = sizeof(display);

                if (!EnumDisplayDevicesW(adapter.DeviceName, displayIndex, &display, 0))
                {
                    break;
                }

                if (!(display.StateFlags & DISPLAY_DEVICE_ACTIVE))
                {
                    continue;
                }

                //if this monitor is in the disconnected array, set the position as null and just refresh the handle
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

                //if the monitor didn't already exist, create a new one
                WindowsMonitor* monitor = new WindowsMonitor(&adapter, &display);
                if (!monitor)
                {
                    return;
                }

                //insert it in the m_Monitors array
                if (insertFirst)
                {
                    m_Monitors.insert(m_Monitors.begin(), monitor);
                }
                else
                {
                    m_Monitors.push_back(monitor);
                }
                insertFirst = false;

                //call the MonitorConnected callback
                if (m_Callbacks.MonitorConnected)
                {
                    m_Callbacks.MonitorConnected((Monitor*)monitor);
                }
            }

            //if an active adapter does not have any display devices add it as a monitor
            if (displayIndex == 0)
            {
                //if this adapter (as monitor) is in the disconnected array, set the position as null and just refresh the handle
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

                //if the monitor didn't already exist, create a new one
                WindowsMonitor* monitor = new WindowsMonitor(&adapter, nullptr);
                if (!monitor)
                {
                    return;
                }

                //insert it in the m_Monitors array
                m_Monitors.push_back(monitor);

                //call the MonitorConnected callback
                if (m_Callbacks.MonitorConnected)
                {
                    m_Callbacks.MonitorConnected((Monitor*)monitor);
                }
            }
        }

        //release remaining monitors in the disconnected array since they were not found
        for (uint32_t d = 0; d < disconnected.size(); d++)
        {
            if (disconnected[d])
            {
                Monitor* monitor = m_Monitors[d];

                //remove this monitor from the m_Monitors array
                m_Monitors.erase(m_Monitors.begin() + d);

                //also remove this monitor from the disconnected array to keep indices in sync with m_Monitors array
                disconnected.erase(disconnected.begin() + d);

                //call the MonitorDisconnected callback
                if (m_Callbacks.MonitorDisconnected)
                {
                    m_Callbacks.MonitorDisconnected((Monitor*)monitor);
                }

                delete monitor;
            }
        }
    }

    bool WindowsPlatform::RegisterWindowClass()
    {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(wc);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = WindowsWindow::WindowProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.lpszClassName = DAIS_WINDOW_CLASS;

        //load user provided icon if available
        wc.hIcon = (HICON)LoadImageW(GetModuleHandleW(nullptr), DAIS_ICON, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
        if (!wc.hIcon)
        {
            //no user provided icon found, load default icon
            wc.hIcon = (HICON)LoadImageW(nullptr, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
        }

        if (!RegisterClassExW(&wc))
        {
            DAIS_ERROR("Failed to register window class!");
            return false;
        }

        return true;
    }

    void WindowsPlatform::UnregisterWindowClass()
    {
        UnregisterClassW(DAIS_WINDOW_CLASS, GetModuleHandleW(nullptr));
    }

    bool WindowsPlatform::CreateHelperWindow()
    {
        m_HelperWindowHandle = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW,
            DAIS_WINDOW_CLASS,
            DAIS_HELPER_WINDOW_TITLE,
            WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            0, 0, 1, 1,
            nullptr, nullptr,
            GetModuleHandleW(nullptr),
            nullptr);

        if (!m_HelperWindowHandle)
        {
            DAIS_TRACE("Failed to create helper window!");
            return false;
        }

        ShowWindow(m_HelperWindowHandle, SW_HIDE);

        //register for HID device notifications
        DEV_BROADCAST_DEVICEINTERFACE_W dbi = {};
        dbi.dbcc_size = sizeof(dbi);
        dbi.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        dbi.dbcc_classguid = GUID_DEVINTERFACE_HID;

        m_DeviceNotificationHandle = RegisterDeviceNotificationW(m_HelperWindowHandle,
            (DEV_BROADCAST_HDR*)&dbi,
            DEVICE_NOTIFY_WINDOW_HANDLE);

        //process messages
        MSG msg;
        while (PeekMessageW(&msg, m_HelperWindowHandle, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        return true;
    }
}
