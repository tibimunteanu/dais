#include "platform/windows/WindowsMonitor.h"

namespace dais
{
    WindowsMonitor::WindowsMonitor(DISPLAY_DEVICEW* adapter, DISPLAY_DEVICEW* display)
        : m_Handle(nullptr), m_DisplayName{0}, m_PublicDisplayName{0}
    {
        std::cout << "[WindowsMonitor] Constructor" << std::endl;

        WindowsBase::WideStringToUTF8(display ? display->DeviceString : adapter->DeviceString, m_Name);

        DEVMODEW dm = { sizeof(dm) };
        EnumDisplaySettingsW(adapter->DeviceName, ENUM_CURRENT_SETTINGS, &dm);

        HDC dc = CreateDCW(L"DISPLAY", adapter->DeviceName, nullptr, nullptr);

        if (WindowsBase::IsWindows8Point1OrGreater())
        {
            m_WidthInMillimeters = GetDeviceCaps(dc, HORZSIZE);
            m_HeightInMillimeters = GetDeviceCaps(dc, VERTSIZE);
        }
        else
        {
            m_WidthInMillimeters = (int)(dm.dmPelsWidth * 25.4f / GetDeviceCaps(dc, LOGPIXELSX));
            m_HeightInMillimeters = (int)(dm.dmPelsHeight * 25.4f / GetDeviceCaps(dc, LOGPIXELSY));
        }

        DeleteDC(dc);

        m_ModesPruned = (adapter->StateFlags & DISPLAY_DEVICE_MODESPRUNED);
        m_ModeChanged = false;

        wcscpy(m_AdapterName, adapter->DeviceName);
        WindowsBase::WideStringToUTF8(adapter->DeviceName, m_PublicAdapterName);

        if (display)
        {
            wcscpy(m_DisplayName, display->DeviceName);
            WindowsBase::WideStringToUTF8(display->DeviceName, m_PublicDisplayName);
        }

        RECT rect =
        {
            dm.dmPosition.x,
            dm.dmPosition.y,
            dm.dmPosition.x + dm.dmPelsWidth,
            dm.dmPosition.y + dm.dmPelsHeight
        };

        m_Handle = nullptr;
        EnumDisplayMonitors(nullptr, &rect, WindowsMonitor::SetHandle, (LPARAM)this);
    }

    WindowsMonitor::~WindowsMonitor()
    {
        std::cout << "[WindowsMonitor] Destructor" << std::endl;
    }

    void WindowsMonitor::PlatformGetPosition(int32_t* x, int32_t* y) const
    {
        std::cout << "[WindowsMonitor] PlatformGetMonitorPosition" << std::endl;
    }

    void WindowsMonitor::PlatformGetWorkarea(int32_t* x, int32_t* y, int32_t* width, int32_t* height) const
    {
        std::cout << "[WindowsMonitor] PlatformGetWorkarea" << std::endl;
    }

    void WindowsMonitor::PlatformGetContentScale(float* xScale, float* yScale) const
    {
        std::cout << "[WindowsMonitor] PlatformGetContentScale" << std::endl;
    }

    VideoMode* WindowsMonitor::PlatformGetVideoModes(int32_t* count)
    {
        std::cout << "[WindowsMonitor] PlatformGetVideoModes" << std::endl;
        return nullptr;
    }

    bool WindowsMonitor::PlatformGetVideoMode(VideoMode* videoMode)
    {
        std::cout << "[WindowsMonitor] PlatformGetVideoMode" << std::endl;
        return false;
    }

    BOOL CALLBACK WindowsMonitor::SetHandle(HMONITOR handle, HDC dc, RECT* rect, LPARAM data)
    {
        MONITORINFOEXW mi = { sizeof(mi) };

        if (GetMonitorInfoW(handle, (MONITORINFO*)&mi))
        {
            WindowsMonitor* monitor = (WindowsMonitor*)data;

            if (wcscmp(mi.szDevice, monitor->m_AdapterName) == 0)
            {
                monitor->m_Handle = handle;
            }
        }

        return TRUE;
    }
}
