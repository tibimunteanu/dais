#include "platform/windows/WindowsMonitor.h"

namespace dais
{
    WindowsMonitor::WindowsMonitor(DISPLAY_DEVICEW* adapter, DISPLAY_DEVICEW* display)
    {
        std::cout << "[WindowsMonitor] Constructor" << std::endl;

        WindowsBase::WideStringToUTF8(display ? display->DeviceString : adapter->DeviceString, m_Name);

        wcscpy(m_AdapterName, adapter->DeviceName);
        WindowsBase::WideStringToUTF8(adapter->DeviceName, m_PublicAdapterName);

        if (display)
        {
            wcscpy(m_DisplayName, display->DeviceName);
            WindowsBase::WideStringToUTF8(display->DeviceName, m_PublicDisplayName);
        }

        m_ModesPruned = (adapter->StateFlags & DISPLAY_DEVICE_MODESPRUNED);
        m_ModeChanged = false;

        //get device properties
        DEVMODEW dm = { sizeof(dm) };
        EnumDisplaySettingsW(adapter->DeviceName, ENUM_CURRENT_SETTINGS, &dm);

        //get dimensions in millimeters
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

        //set handle
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

        DEVMODEW dm = {};
        dm.dmSize = sizeof(dm);

        EnumDisplaySettingsExW(m_AdapterName, ENUM_CURRENT_SETTINGS, &dm, EDS_ROTATEDMODE);

        if (x)
        {
            *x = dm.dmPosition.x;
        }
        if (y)
        {
            *y = dm.dmPosition.y;
        }
    }

    void WindowsMonitor::PlatformGetWorkarea(int32_t* x, int32_t* y, int32_t* width, int32_t* height) const
    {
        std::cout << "[WindowsMonitor] PlatformGetWorkarea" << std::endl;

        MONITORINFO mi = {};
        mi.cbSize = sizeof(mi);

        GetMonitorInfo(m_Handle, &mi);

        if (x)
        {
            *x = mi.rcWork.left;
        }
        if (y)
        {
            *y = mi.rcWork.top;
        }
        if (width)
        {
            *width = mi.rcWork.right - mi.rcWork.left;
        }
        if (height)
        {
            *height = mi.rcWork.bottom - mi.rcWork.top;
        }
    }

    void WindowsMonitor::PlatformGetContentScale(float* xScale, float* yScale) const
    {
        std::cout << "[WindowsMonitor] PlatformGetContentScale" << std::endl;

        UINT xdpi, ydpi;

        if (WindowsBase::IsWindows8Point1OrGreater())
        {
            WindowsBase::Libs.Shcore.GetDpiForMonitor(m_Handle, MDT_EFFECTIVE_DPI, &xdpi, &ydpi);
        }
        else
        {
            const HDC dc = GetDC(nullptr);
            xdpi = GetDeviceCaps(dc, LOGPIXELSX);
            ydpi = GetDeviceCaps(dc, LOGPIXELSY);
            ReleaseDC(nullptr, dc);
        }

        if (xScale)
        {
            *xScale = xdpi / (float)USER_DEFAULT_SCREEN_DPI;
        }
        if (yScale)
        {
            *yScale = ydpi / (float)USER_DEFAULT_SCREEN_DPI;
        }
    }

    void WindowsMonitor::PlatformGetVideoModes(std::vector<VideoMode*>& videoModes)
    {
        std::cout << "[WindowsMonitor] PlatformGetVideoModes" << std::endl;

        int32_t videoModeIndex = 0;

        for (;;)
        {
            DEVMODEW dm = {};
            dm.dmSize = sizeof(dm);

            if (!EnumDisplaySettingsW(m_AdapterName, videoModeIndex, &dm))
            {
                break;
            }

            videoModeIndex++;

            //skip modes with less than 15 BPP
            if (dm.dmBitsPerPel < 15)
            {
                continue;
            }

            int32_t redBits, greenBits, blueBits;
            Monitor::SplitBPP(dm.dmBitsPerPel, &redBits, &greenBits, &blueBits);

            //skip duplicate modes
            int32_t i;
            for (i = 0; i < m_VideoModes.size(); i++)
            {
                VideoMode existingVideoMode = *m_VideoModes[i];

                if (existingVideoMode.Width == dm.dmPelsWidth
                    && existingVideoMode.Height == dm.dmPelsHeight
                    && existingVideoMode.RefreshRate == dm.dmDisplayFrequency
                    && existingVideoMode.RedBits == redBits
                    && existingVideoMode.GreenBits == greenBits
                    && existingVideoMode.BlueBits == blueBits)
                {
                    break;
                }
            }
            if (i < m_VideoModes.size())
            {
                continue;
            }

            //skip modes not supported by the connected displays
            if (m_ModesPruned
                && (ChangeDisplaySettingsExW(m_AdapterName, &dm, nullptr, CDS_TEST, nullptr) != DISP_CHANGE_SUCCESSFUL))
            {
                continue;
            }

            //add a new video mode
            VideoMode* videoMode = new VideoMode();
            videoMode->Width = dm.dmPelsWidth;
            videoMode->Height = dm.dmPelsHeight;
            videoMode->RefreshRate = dm.dmDisplayFrequency;
            videoMode->RedBits = redBits;
            videoMode->GreenBits = greenBits;
            videoMode->BlueBits = blueBits;

            m_VideoModes.push_back(videoMode);
        }

        //if no valid modes were found, add the current mode
        if (!m_VideoModes.size())
        {
            VideoMode* videoMode = new VideoMode();
            PlatformGetVideoMode(videoMode);
            m_VideoModes.push_back(videoMode);
        }
    }

    void WindowsMonitor::PlatformGetVideoMode(VideoMode* videoMode)
    {
        std::cout << "[WindowsMonitor] PlatformGetVideoMode" << std::endl;

        DEVMODEW dm = {};
        dm.dmSize = sizeof(dm);

        EnumDisplaySettingsW(m_AdapterName, ENUM_CURRENT_SETTINGS, &dm);

        videoMode->Width = dm.dmPelsWidth;
        videoMode->Height = dm.dmPelsHeight;
        videoMode->RefreshRate = dm.dmDisplayFrequency;

        Monitor::SplitBPP(dm.dmBitsPerPel, &videoMode->RedBits, &videoMode->GreenBits, &videoMode->BlueBits);
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
