#pragma once

#include "platform/windows/WindowsBase.h"
#include "engine/core/Monitor.h"

namespace dais
{
    class WindowsMonitor : public Monitor
    {
    public:
        HMONITOR m_Handle = nullptr;
        WCHAR m_AdapterName[32] = {};
        WCHAR m_DisplayName[32] = {};
        char m_PublicAdapterName[32] = {};
        char m_PublicDisplayName[32] = {};
        bool m_ModesPruned = false; //the device has more display modes than its output devices support
        bool m_ModeChanged = false;

    public:
        static BOOL CALLBACK SetHandle(HMONITOR handle, HDC dc, RECT* rect, LPARAM data);
        static void GetContentScale(HMONITOR handle, float* xScale, float* yScale);

    public:
        WindowsMonitor(DISPLAY_DEVICEW* adapter, DISPLAY_DEVICEW* display);
        virtual ~WindowsMonitor();

    private:
        void PlatformGetPosition(int32_t* x, int32_t* y) const override;
        void PlatformGetWorkarea(int32_t* x, int32_t* y, int32_t* width, int32_t* height) const override;
        void PlatformGetContentScale(float* xScale, float* yScale) const override;
        void PlatformGetVideoModes(std::vector<VideoMode*>& videoModes) override;
        void PlatformGetVideoMode(VideoMode* videoMode) override;
        void PlatformSetVideoMode(const VideoMode* videoMode) override;
        void PlatformRestoreVideoMode() override;
        bool PlatformGetGammaRamp(GammaRamp* ramp) override;
        void PlatformSetGammaRamp(const GammaRamp* ramp) override;
    };
}
