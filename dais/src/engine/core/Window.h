#pragma once

#include "engine/core/Base.h"
#include "engine/core/Monitor.h"

namespace dais
{
    struct WindowConfig
    {
        std::string title;
        int32_t width;
        int32_t height;
        bool decorated;
        bool visible;
        bool focused;
        bool focusOnShow;
        bool autoIconify;
        bool floating;
        bool maximized;
        bool resizable;
        bool centerCursor;
        bool mousePassthrough;
        bool scaleToMonitor;
    };

    class Window
    {
    protected:
        std::string m_Title = {};
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        bool m_Decorated = false;
        bool m_Visible = false;
        bool m_Focused = false;
        bool m_FocusOnShow = false;
        bool m_AutoIconify = false;
        bool m_Floating = false;
        bool m_Maximized = false;
        bool m_Resizable = false;
        bool m_CenterCursor = false;
        bool m_MousePassthrough = false;
        bool m_ScaleToMonitor = false;
        Monitor* m_Monitor = nullptr;


    protected:
        Window(WindowConfig config, Monitor* monitor);

    public:
        virtual ~Window();

        virtual void* GetHandle() const = 0; // Get native window handle

        virtual void* GetUserData() const = 0;
        virtual void SetUserData(void* data) = 0;

        virtual uint32_t GetMonitorCount() const = 0;
        virtual uint32_t GetMonitor() const = 0;
        virtual void SetMonitor(uint32_t monitor) = 0;

        virtual const std::string& GetTitle() const = 0;
        virtual void SetTitle(const std::string& title) = 0;

        virtual void GetSize(uint32_t* width, uint32_t* height) const = 0;
        virtual void SetSize(uint32_t width, uint32_t height) = 0;

        virtual void GetPosition(uint32_t* x, uint32_t* y) const = 0;
        virtual void SetPosition(uint32_t x, uint32_t y) = 0;

        virtual bool IsFullscreen() const = 0;
        virtual void SetFullscreen(bool fullscreen) = 0; 

        virtual bool IsMaximized() const = 0;
        virtual void Maximize() = 0;

        virtual bool IsMinimized() const = 0;
        virtual void Minimize() = 0;

        virtual void Restore() = 0;
        virtual bool ShouldClose() const = 0; // Close icon pressed
        virtual void Close() = 0;

    public:
        virtual void PlatformShow() = 0;
        virtual void PlatformHide() = 0;
        virtual void PlatformFocus() = 0;
        virtual void PlatformGetContentScale(float* xScale, float* yScale) = 0;

    public:
        static Window* Create(WindowConfig config, Monitor* monitor);
    };
}
