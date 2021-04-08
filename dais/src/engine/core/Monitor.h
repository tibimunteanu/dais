#pragma once

#include "engine/core/Base.h"

namespace dais
{
    struct VideoMode
    {
        int32_t Width;
        int32_t Height;
        int32_t RedBits;
        int32_t GreenBits;
        int32_t BlueBits;
        int32_t RefreshRate;
    };

    class Monitor
    {
    protected:
        std::string m_Name;

        int32_t m_WidthInMillimeters;
        int32_t m_HeightInMillimeters;

        VideoMode m_CurrentVideoMode;
        uint32_t m_VideoModeCount;
        VideoMode* m_VideoModes;

    protected:
        Monitor();

    public:
        virtual ~Monitor();

        const std::string& GetName() const;
        void GetPosition(int32_t* x, int32_t* y) const;
        void GetWorkarea(int32_t* x, int32_t* y, int32_t* width, int32_t* height) const;
        void GetContentScale(float* xScale, float* yScale) const;
        void GetPhysicalSize(int32_t* widthInMillimeters, int32_t* heightInMillimeters) const;
        VideoMode* GetVideoModes(int32_t* count);
        VideoMode* GetVideoMode();

    private:
        bool RefreshVideoModes();

    private:
        //NOTE: should the platform calls always be given allocated memory to fill so that the ownership is kept simple?
        //      could use the vulkan model with 2 calls, first with nullptr as data to query the count, and second call to fill the array
        //      the subtlety is that the platform layer might need to allocate platform specific structs, but maybe it can translate them
        //      to platform agnostic structs and free the platform specific ones after it's done with them
        virtual void PlatformGetPosition(int32_t* x, int32_t* y) const = 0;
        virtual void PlatformGetWorkarea(int32_t* x, int32_t* y, int32_t* width, int32_t* height) const = 0;
        virtual void PlatformGetContentScale(float* xScale, float* yScale) const = 0;
        virtual VideoMode* PlatformGetVideoModes(int32_t* count) = 0;
        virtual bool PlatformGetVideoMode(VideoMode* videoMode) = 0;
    };
}
