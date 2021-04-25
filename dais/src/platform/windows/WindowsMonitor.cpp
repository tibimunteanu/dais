#include "platform/windows/WindowsPlatform.h"

namespace dais
{
    //////////////////////////////////////// STATIC ///////////////////////////////////////////

    BOOL CALLBACK WindowsMonitor::SetHandle(HMONITOR handle, HDC dc, RECT* rect, LPARAM data)
    {
        MONITORINFOEXW mi = {};
        mi.cbSize = sizeof(mi);

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

    void WindowsMonitor::GetContentScale(HMONITOR handle, float* xScale, float* yScale)
    {
        UINT xdpi, ydpi;

        if (WindowsPlatform::IsWindows8Point1OrGreater())
        {
            WindowsPlatform::s_Libs.shcore.GetDpiForMonitor(handle, MDT_EFFECTIVE_DPI, &xdpi, &ydpi);
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



    ////////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////

    WindowsMonitor::WindowsMonitor(DISPLAY_DEVICEW* adapter, DISPLAY_DEVICEW* display)
    {
        WindowsPlatform::WideStringToUTF8(display ? display->DeviceString : adapter->DeviceString, m_Name);

        wcscpy(m_AdapterName, adapter->DeviceName);
        WindowsPlatform::WideStringToUTF8(adapter->DeviceName, m_PublicAdapterName);

        if (display)
        {
            wcscpy(m_DisplayName, display->DeviceName);
            WindowsPlatform::WideStringToUTF8(display->DeviceName, m_PublicDisplayName);
        }

        m_ModesPruned = (adapter->StateFlags & DISPLAY_DEVICE_MODESPRUNED);
        m_ModeChanged = false;

        //get device properties
        DEVMODEW dm = {};
        dm.dmSize = sizeof(dm);
        EnumDisplaySettingsW(adapter->DeviceName, ENUM_CURRENT_SETTINGS, &dm);

        //get dimensions in millimeters
        HDC dc = CreateDCW(L"DISPLAY", adapter->DeviceName, NULL, NULL);

        if (WindowsPlatform::IsWindows8Point1OrGreater())
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
    }



    ///////////////////////////////////// PLATFORM API ////////////////////////////////////////

    void WindowsMonitor::PlatformGetPosition(int32_t* x, int32_t* y) const
    {
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
        WindowsMonitor::GetContentScale(m_Handle, xScale, yScale);
    }


    void WindowsMonitor::PlatformGetVideoModes(std::vector<VideoMode*>& videoModes)
    {
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

                if (existingVideoMode.width == dm.dmPelsWidth
                    && existingVideoMode.height == dm.dmPelsHeight
                    && existingVideoMode.refreshRate == dm.dmDisplayFrequency
                    && existingVideoMode.redBits == redBits
                    && existingVideoMode.greenBits == greenBits
                    && existingVideoMode.blueBits == blueBits)
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
                && (ChangeDisplaySettingsExW(m_AdapterName, &dm, NULL, CDS_TEST, NULL) != DISP_CHANGE_SUCCESSFUL))
            {
                continue;
            }

            //add a new video mode
            VideoMode* videoMode = new VideoMode();
            videoMode->width = dm.dmPelsWidth;
            videoMode->height = dm.dmPelsHeight;
            videoMode->refreshRate = dm.dmDisplayFrequency;
            videoMode->redBits = redBits;
            videoMode->greenBits = greenBits;
            videoMode->blueBits = blueBits;

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
        DEVMODEW dm = {};
        dm.dmSize = sizeof(dm);

        EnumDisplaySettingsW(m_AdapterName, ENUM_CURRENT_SETTINGS, &dm);

        videoMode->width = dm.dmPelsWidth;
        videoMode->height = dm.dmPelsHeight;
        videoMode->refreshRate = dm.dmDisplayFrequency;

        Monitor::SplitBPP(dm.dmBitsPerPel, &videoMode->redBits, &videoMode->greenBits, &videoMode->blueBits);
    }

    void WindowsMonitor::PlatformSetVideoMode(const VideoMode* videoMode)
    {
        const VideoMode* closestVideoMode = GetClosestVideoMode(videoMode);
        VideoMode* currentVideoMode = GetVideoMode();

        if (*currentVideoMode == *closestVideoMode)
        {
            return;
        }

        DEVMODEW dm = {};
        dm.dmSize = sizeof(dm);
        dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;
        dm.dmPelsWidth = closestVideoMode->width;
        dm.dmPelsHeight = closestVideoMode->height;
        dm.dmBitsPerPel = closestVideoMode->redBits + closestVideoMode->greenBits + closestVideoMode->blueBits;
        dm.dmDisplayFrequency = closestVideoMode->refreshRate;

        if (dm.dmBitsPerPel < 15 || dm.dmBitsPerPel >= 24)
        {
            dm.dmBitsPerPel = 32;
        }

        LONG result = ChangeDisplaySettingsExW(m_AdapterName, &dm, NULL, CDS_FULLSCREEN, NULL);
        if (result == DISP_CHANGE_SUCCESSFUL)
        {
            m_ModeChanged = true;
        }
        else
        {
            std::string error = "Unknown error";
            if (result == DISP_CHANGE_BADDUALVIEW) error = "The system uses DualView";
            else if (result == DISP_CHANGE_BADFLAGS) error = "Invalid flags";
            else if (result == DISP_CHANGE_BADMODE) error = "Video mode not supported";
            else if (result == DISP_CHANGE_BADPARAM) error = "Invalid parameter";
            else if (result == DISP_CHANGE_FAILED) error = "Video mode failed";
            else if (result == DISP_CHANGE_NOTUPDATED) error = "Failed to write to registry";
            else if (result == DISP_CHANGE_RESTART) error = "Computer restart required";

            DAIS_ERROR("Failed to set video mode: %s", error);
        }
    }

    void WindowsMonitor::PlatformRestoreVideoMode()
    {
        if (m_ModeChanged)
        {
            ChangeDisplaySettingsExW(m_AdapterName, NULL, NULL, CDS_FULLSCREEN, NULL);
            m_ModeChanged = false;
        }
    }


    bool WindowsMonitor::PlatformGetGammaRamp(GammaRamp* ramp)
    {
        HDC dc = CreateDCW(L"DISPLAY", m_AdapterName, NULL, NULL);

        WORD values[3][256];
        GetDeviceGammaRamp(dc, values);

        DeleteDC(dc);

        ramp->Clear();
        ramp->size = 256;

        for (int i = 0; i < 256; i++)
        {
            ramp->red.push_back(values[0][i]);
            ramp->green.push_back(values[1][i]);
            ramp->blue.push_back(values[2][i]);
        }

        return true;
    }

    void WindowsMonitor::PlatformSetGammaRamp(const GammaRamp* ramp)
    {
        WORD values[3][256];

        if (ramp->size != 256)
        {
            DAIS_ERROR("Gamma ramp size must be 256!");
            return;
        }

        for (int i = 0; i < 256; i++)
        {
            values[0][i] = ramp->red[i];
            values[1][i] = ramp->green[i];
            values[2][i] = ramp->blue[i];
        }

        HDC dc = CreateDCW(L"DISPLAY", m_AdapterName, NULL, NULL);
        SetDeviceGammaRamp(dc, values);
        DeleteDC(dc);
    }
}
