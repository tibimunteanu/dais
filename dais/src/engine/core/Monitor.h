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

        bool operator==(const VideoMode& other)
        {
            return Width == other.Width
                && Height == other.Height
                && RedBits == other.RedBits
                && GreenBits == other.GreenBits
                && BlueBits == other.BlueBits
                && RefreshRate == other.RefreshRate;
        }

        bool operator<(const VideoMode& other) const
        {
            const int32_t bpp = RedBits + GreenBits + BlueBits;
            const int32_t otherBpp = other.RedBits + other.GreenBits + other.BlueBits;
            const int32_t area = Width * Height;
            const int32_t otherArea = other.Width * other.Height;

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
            if (Width != other.Width)
            {
                return Width < other.Width;
            }

            //lastly sort on refresh rate
            return RefreshRate < other.RefreshRate;
        }

        friend std::ostream& operator<<(std::ostream& os, const VideoMode& videoMode)
        {
            os
                << videoMode.Width << " x " << videoMode.Height
                << " (" << videoMode.RefreshRate << "Hz)"
                << " [r" << videoMode.RedBits << ", g" << videoMode.GreenBits << ", b" << videoMode.BlueBits << "]";

            return os;
        }
    };

    struct GammaRamp
    {
        std::vector<uint16_t> Red;
        std::vector<uint16_t> Green;
        std::vector<uint16_t> Blue;
        uint32_t Size;

        GammaRamp() = default;

        GammaRamp(uint32_t size)
        {
            Red.reserve(size);
            Green.reserve(size);
            Blue.reserve(size);
            Size = size;
        }

        void Clear()
        {
            Red.clear();
            Green.clear();
            Blue.clear();
            Size = 0;
        }

        bool IsValid()
        {
            return Size > 0
                && Red.size() == Size
                && Green.size() == Size
                && Blue.size() == Size;
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

        GammaRamp m_OriginalGammaRamp;
        GammaRamp m_CurrentGammaRamp;

    protected:
        Monitor();

    public:
        virtual ~Monitor();

        const std::string& GetName() const;
        void GetPosition(int32_t* x, int32_t* y) const;
        void GetWorkarea(int32_t* x, int32_t* y, int32_t* width, int32_t* height) const;
        void GetContentScale(float* xScale, float* yScale) const;
        void GetPhysicalSize(int32_t* widthInMillimeters, int32_t* heightInMillimeters) const;
        const std::vector<VideoMode*>& GetVideoModes();
        VideoMode* GetVideoMode();
        //void SetVideoMode(const VideoMode* desired);
        //void RestoreVideoMode();
        void SetGamma(float gamma);
        const GammaRamp* GetGammaRamp();
        void SetGammaRamp(GammaRamp* ramp);

    private:
        void RefreshVideoModes();

    private:
        virtual void PlatformGetPosition(int32_t* x, int32_t* y) const = 0;
        virtual void PlatformGetWorkarea(int32_t* x, int32_t* y, int32_t* width, int32_t* height) const = 0;
        virtual void PlatformGetContentScale(float* xScale, float* yScale) const = 0;
        virtual void PlatformGetVideoModes(std::vector<VideoMode*>& videoModes) = 0;
        virtual void PlatformGetVideoMode(VideoMode* videoMode) = 0;
        //virtual void PlatformSetVideoMode(const VideoMode* desired);
        //virtual void PlatformRestoreVideoMode();
        virtual bool PlatformGetGammaRamp(GammaRamp* ramp);
        virtual void PlatformSetGammaRamp(const GammaRamp* ramp);

    protected:
        static void SplitBPP(int32_t bpp, int32_t* red, int32_t* green, int32_t* blue);
    };
}
