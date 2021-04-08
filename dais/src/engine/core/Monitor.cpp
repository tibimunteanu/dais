#include "engine/core/Monitor.h"

namespace dais
{
    Monitor::Monitor()
    {
        std::cout << "[Monitor] Constructor" << std::endl;

        m_Name = "Invalid Monitor";
        m_WidthInMillimeters = 0;
        m_HeightInMillimeters = 0;
        m_VideoModeCount = 0;
        m_VideoModes = nullptr;
        m_CurrentVideoMode = {};
    }

    Monitor::~Monitor()
    {
        std::cout << "[Monitor] Destructor" << std::endl;
    }

    const std::string& Monitor::GetName() const
    {
        return m_Name;
    }

    void Monitor::GetPosition(int32_t* x, int32_t* y) const
    {
        return PlatformGetPosition(x, y);
    }

    void Monitor::GetWorkarea(int32_t* x, int32_t* y, int32_t* width, int32_t* height) const
    {
        return PlatformGetWorkarea(x, y, width, height);
    }

    void Monitor::GetContentScale(float* xScale, float* yScale) const
    {
        return PlatformGetContentScale(xScale, yScale);
    }

    void Monitor::GetPhysicalSize(int32_t* widthInMillimeters, int32_t* heightInMillimeters) const
    {
        *widthInMillimeters = m_WidthInMillimeters;
        *heightInMillimeters = m_HeightInMillimeters;
    }

    bool Monitor::RefreshVideoModes()
    {
        if (m_VideoModes)
        {
            //NOTE: since video modes will not change at runtime, this method's purpose is only for lazy loading
            return true;
        }

        int32_t modeCount;
        VideoMode* modes = PlatformGetVideoModes(&modeCount);
        if (!modes)
        {
            return false;
        }

        //TODO: sort modes

        free(m_VideoModes);

        m_VideoModes = modes;
        m_VideoModeCount = modeCount;

        return true;
    }

    VideoMode* Monitor::GetVideoModes(int32_t* count)
    {
        std::cout << "[Monitor] GetVideoModes" << std::endl;

        if (!RefreshVideoModes())
        {
            *count = 0;
            return nullptr;
        }

        *count = m_VideoModeCount;
        return m_VideoModes;
    }

    VideoMode* Monitor::GetVideoMode()
    {
        if (!PlatformGetVideoMode(&m_CurrentVideoMode))
        {
            return nullptr;
        }
        return &m_CurrentVideoMode;
    }
}
