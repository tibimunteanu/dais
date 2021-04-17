#pragma once

#include "engine/core/Base.h"

namespace dais
{
    class Window;

    struct VideoMode
    {
        int32_t width;
        int32_t height;
        int32_t redBits;
        int32_t greenBits;
        int32_t blueBits;
        int32_t refreshRate;

        bool operator==(const VideoMode& other)
        {
            return width == other.width
                && height == other.height
                && redBits == other.redBits
                && greenBits == other.greenBits
                && blueBits == other.blueBits
                && refreshRate == other.refreshRate;
        }

        bool operator<(const VideoMode& other) const
        {
            const int32_t bpp = redBits + greenBits + blueBits;
            const int32_t otherBpp = other.redBits + other.greenBits + other.blueBits;
            const int32_t area = width * height;
            const int32_t otherArea = other.width * other.height;

            //first sort on color bits per pixel
            if (bpp != otherBpp)
            {
                return bpp < otherBpp;
            }

            //then sort on screen area
            if (area != otherArea)
            {
                return area < otherArea;
            }

            //then sort on width
            if (width != other.width)
            {
                return width < other.width;
            }

            //lastly sort on refresh rate
            return refreshRate < other.refreshRate;
        }

        friend std::ostream& operator<<(std::ostream& os, const VideoMode& videoMode)
        {
            os
                << videoMode.width << " x " << videoMode.height
                << " (" << videoMode.refreshRate << "Hz)"
                << " [r" << videoMode.redBits << ", g" << videoMode.greenBits << ", b" << videoMode.blueBits << "]";

            return os;
        }
    };

    struct GammaRamp
    {
        std::vector<uint16_t> red;
        std::vector<uint16_t> green;
        std::vector<uint16_t> blue;
        uint32_t size;

        GammaRamp() = default;

        GammaRamp(uint32_t size)
        {
            red.reserve(size);
            green.reserve(size);
            blue.reserve(size);
            this->size = size;
        }

        void Clear()
        {
            red.clear();
            green.clear();
            blue.clear();
            size = 0;
        }

        bool IsValid()
        {
            return size > 0
                && red.size() == size
                && green.size() == size
                && blue.size() == size;
        }
    };

    class Monitor
    {
    protected:
        std::string m_Name = {};

        int32_t m_WidthInMillimeters = 0;
        int32_t m_HeightInMillimeters = 0;

        VideoMode m_CurrentVideoMode = {};
        std::vector<VideoMode*> m_VideoModes = {};

        GammaRamp m_OriginalGammaRamp = {};
        GammaRamp m_CurrentGammaRamp = {};

        Window* m_Window = nullptr;

    protected:
        static void SplitBPP(int32_t bpp, int32_t* red, int32_t* green, int32_t* blue);

    protected:
        Monitor();

    public:
        virtual ~Monitor();

    public:
        const std::string& GetName() const;
        void GetPosition(int32_t* x, int32_t* y) const;
        void GetWorkarea(int32_t* x, int32_t* y, int32_t* width, int32_t* height) const;
        void GetContentScale(float* xScale, float* yScale) const;
        void GetPhysicalSize(int32_t* widthInMillimeters, int32_t* heightInMillimeters) const;
        const std::vector<VideoMode*>& GetVideoModes();
        VideoMode* GetVideoMode();
        void SetVideoMode(const VideoMode* videoMode);
        void RestoreVideoMode();
        void SetGamma(float gamma);
        const GammaRamp* GetGammaRamp();
        void SetGammaRamp(GammaRamp* ramp);
        void RestoreOriginalGammaRamp();
        Window* GetWindow() const;
        void SetWindow(Window* window);

    protected:
        bool RefreshVideoModes();
        const VideoMode* GetClosestVideoMode(const VideoMode* videoMode);

    private:
        virtual void PlatformGetPosition(int32_t* x, int32_t* y) const = 0;
        virtual void PlatformGetWorkarea(int32_t* x, int32_t* y, int32_t* width, int32_t* height) const = 0;
        virtual void PlatformGetContentScale(float* xScale, float* yScale) const = 0;
        virtual void PlatformGetVideoModes(std::vector<VideoMode*>& videoModes) = 0;
        virtual void PlatformGetVideoMode(VideoMode* videoMode) = 0;
        virtual void PlatformSetVideoMode(const VideoMode* videoMode) = 0;
        virtual void PlatformRestoreVideoMode() = 0;
        virtual bool PlatformGetGammaRamp(GammaRamp* ramp) = 0;
        virtual void PlatformSetGammaRamp(const GammaRamp* ramp) = 0;
    };
}
