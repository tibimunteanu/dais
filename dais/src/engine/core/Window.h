#pragma once

#include "engine/core/Base.h"

namespace dais
{
    class Monitor;

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
        int32_t refreshRate;
    };

    struct FramebufferConfig
    {
        int32_t redBits;
        int32_t greenBits;
        int32_t blueBits;
        int32_t alphaBits;
        int32_t depthBits;
        int32_t stencilBits;
        bool sRGB;
    };

    class Window
    {
    protected:
        std::string m_Title = {};
        int32_t m_Width = 0;
        int32_t m_Height = 0;
        bool m_Floating = false;
        bool m_Decorated = false;
        bool m_Resizable = false;
        bool m_FocusOnShow = false;
        bool m_AutoIconify = false;
        bool m_MousePassthrough = false;
        bool m_ShouldClose = false;

        VideoMode m_VideoMode = {};
        Monitor* m_Monitor = nullptr;

    protected:
        Window(WindowConfig config, FramebufferConfig fbConfig, Monitor* monitor);

    public:
        static Window* Create(WindowConfig config, FramebufferConfig fbConfig, Monitor* monitor);

    public:
        virtual ~Window();

    public:
        bool IsMaximized() const;
        bool IsMinimized() const;
        bool IsVisible() const;
        bool IsFocused() const;
        bool IsHovered() const;
        bool IsFloating() const;
        bool IsDecorated() const;
        bool IsResizable() const;
        bool IsFocusOnShow() const;
        bool IsAutoIconify() const;
        bool IsMousePassThrough() const;
        bool ShouldClose() const; // Close icon pressed

        const std::string& GetTitle() const;
        void GetPosition(int32_t* x, int32_t* y) const;
        void GetSize(int32_t* width, int32_t* height);
        void GetFrameSize(int32_t* left, int32_t* top, int32_t* right, int32_t* bottom);
        void GetContentScale(float* xScale, float* yScale);
        const Monitor* GetMonitor() const;
        void* GetNativeHandle() const;

        void SetTitle(const std::string& title);
        void SetPosition(int32_t x, int32_t y);
        void SetSize(int32_t width, int32_t height);
        void SetFloating(bool value);
        void SetDecorated(bool value);
        void SetResizable(bool value);
        void SetFocusOnShow(bool value);
        void SetAutoIconify(bool value);
        void SetMousePassThrough(bool value);
        void SetShouldClose(bool value);
        void SetMonitor(Monitor* monitor, int32_t x, int32_t y, int32_t width, int32_t height, int32_t refreshRate);

        void Maximize();
        void Minimize();
        void Restore();
        void Show();
        void Hide();
        void Focus();
        void RequestAttention();

    public:
        virtual bool PlatformIsMaximized() const = 0;
        virtual bool PlatformIsMinimized() const = 0;
        virtual bool PlatformIsVisible() const = 0;
        virtual bool PlatformIsHovered() const = 0;
        virtual bool PlatformIsFocused() const = 0;

        virtual void PlatformGetPosition(int32_t* x, int32_t* y) const = 0;
        virtual void PlatformGetSize(int32_t* width, int32_t* height) const = 0;
        virtual void PlatformGetFrameSize(int32_t* left, int32_t* top, int32_t* right, int32_t* bottom) const = 0;
        virtual void PlatformGetContentScale(float* xScale, float* yScale) = 0;
        virtual void* PlatformGetHandle() const = 0;

        virtual void PlatformSetTitle(const std::string& title) = 0;
        virtual void PlatformSetPosition(int32_t x, int32_t y) = 0;
        virtual void PlatformSetSize(int32_t width, int32_t height) = 0;
        virtual void PlatformSetDecorated(bool value) = 0;
        virtual void PlatformSetFloating(bool value) = 0;
        virtual void PlatformSetResizable(bool value) = 0;
        virtual void PlatformSetMousePassThrough(bool value) = 0;
        virtual void PlatformSetMonitor(Monitor* monitor, int32_t x, int32_t y, int32_t width, int32_t height, int32_t refreshRate) = 0;

        virtual void PlatformMaximize() = 0;
        virtual void PlatformMinimize() = 0;
        virtual void PlatformRestore() = 0;
        virtual void PlatformShow() = 0;
        virtual void PlatformHide() = 0;
        virtual void PlatformRequestAttention() = 0;
        virtual void PlatformFocus() = 0;
    };
}
