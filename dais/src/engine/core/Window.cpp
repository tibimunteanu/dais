#include "engine/core/Window.h"

namespace dais
{
    ////////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////

    Window::Window(WindowConfig config, FramebufferConfig fbConfig, Monitor* monitor)
    {
        DAIS_TRACE("[Window] Constructor");

        m_Title = config.title;
        m_Width = config.width;
        m_Height = config.height;
        m_Decorated = config.decorated;
        m_FocusOnShow = config.focusOnShow;
        m_AutoIconify = config.autoIconify;
        m_Floating = config.floating;
        m_Resizable = config.resizable;
        m_MousePassthrough = config.mousePassthrough;

        m_VideoMode = {};
        m_VideoMode.width = config.width;
        m_VideoMode.height = config.height;
        m_VideoMode.redBits = fbConfig.redBits;
        m_VideoMode.greenBits = fbConfig.greenBits;
        m_VideoMode.blueBits = fbConfig.blueBits;
        m_VideoMode.refreshRate = config.refreshRate;

        m_Monitor = monitor;
    }

    Window::~Window()
    {
        DAIS_TRACE("[Window] Destructor");

        //TODO: Platform::remove from window array
    }



    /////////////////////////////////////// PUBLIC API ////////////////////////////////////////

    bool Window::IsMaximized() const
    {
        DAIS_TRACE("[Window] IsMaximized");

        return PlatformIsMaximized();
    }

    bool Window::IsMinimized() const
    {
        DAIS_TRACE("[Window] IsMinimized");

        return PlatformIsMinimized();
    }

    bool Window::IsVisible() const
    {
        DAIS_TRACE("[Window] IsVisible");

        return PlatformIsVisible();
    }

    bool Window::IsFocused() const
    {
        DAIS_TRACE("[Window] IsFocused");

        return PlatformIsFocused();
    }

    bool Window::IsHovered() const
    {
        DAIS_TRACE("[Window] IsHovered");

        return PlatformIsHovered();
    }

    bool Window::IsFloating() const
    {
        DAIS_TRACE("[Window] IsFloating");

        return m_Floating;
    }

    bool Window::IsDecorated() const
    {
        DAIS_TRACE("[Window] IsDecorated");

        return m_Decorated;
    }

    bool Window::IsResizable() const
    {
        DAIS_TRACE("[Window] IsResizable");

        return m_Resizable;
    }

    bool Window::IsFocusOnShow() const
    {
        DAIS_TRACE("[Window] IsFocusOnShow");

        return m_FocusOnShow;
    }

    bool Window::IsAutoIconify() const
    {
        DAIS_TRACE("[Window] IsAutoIconify");

        return m_AutoIconify;
    }

    bool Window::IsMousePassThrough() const
    {
        DAIS_TRACE("[Window] IsMousePassThrough");

        return m_MousePassthrough;
    }

    bool Window::ShouldClose() const
    {
        //DAIS_TRACE("[Window] ShouldClose");

        return m_ShouldClose;
    }


    const std::string& Window::GetTitle() const
    {
        DAIS_TRACE("[Window] GetTitle");

        return m_Title;
    }

    void Window::GetPosition(int32_t* x, int32_t* y) const
    {
        DAIS_TRACE("[Window] GetPosition");

        if (x) *x = 0;
        if (y) *y = 0;

        PlatformGetPosition(x, y);
    }

    void Window::GetSize(int32_t* width, int32_t* height)
    {
        DAIS_TRACE("[Window] GetSize");

        if (width) *width = 0;
        if (height) *height = 0;

        PlatformGetSize(width, height);
    }

    void Window::GetFrameSize(int32_t* left, int32_t* top, int32_t* right, int32_t* bottom)
    {
        DAIS_TRACE("[Window] GetFrameSize");

        if (left) *left = 0;
        if (top) *top = 0;
        if (right) *right = 0;
        if (bottom) *bottom = 0;

        PlatformGetFrameSize(left, top, right, bottom);
    }

    void Window::GetContentScale(float* xScale, float* yScale)
    {
        DAIS_TRACE("[Window] GetContentScale");

        if (xScale) *xScale = 0.0f;
        if (yScale) *yScale = 0.0f;

        PlatformGetContentScale(xScale, yScale);
    }

    const Monitor* Window::GetMonitor() const
    {
        DAIS_TRACE("[Window] GetMonitor");

        return m_Monitor;
    }

    void* Window::GetNativeHandle() const
    {
        DAIS_TRACE("[Window] GetNativeHandle");

        return PlatformGetHandle();
    }


    void Window::SetTitle(const std::string& title)
    {
        DAIS_TRACE("[Window] SetTitle");

        if (title.empty())
        {
            DAIS_ERROR("Invalid window title '%s'!", title.c_str());
            return;
        }

        PlatformSetTitle(title);
    }

    void Window::SetPosition(int32_t x, int32_t y)
    {
        DAIS_TRACE("[Window] SetPosition");

        if (m_Monitor)
        {
            return;
        }

        PlatformSetPosition(x, y);
    }

    void Window::SetSize(int32_t width, int32_t height)
    {
        DAIS_TRACE("[Window] SetSize");

        if (width < 0
            || height < 0)
        {
            DAIS_ERROR("Invalid window size %i x %i!", width, height);
        }

        m_VideoMode.width = width;
        m_VideoMode.height = height;

        PlatformSetSize(width, height);
    }

    void Window::SetFloating(bool value)
    {
        DAIS_TRACE("[Window] SetFloating");

        m_Floating = value;

        if (!m_Monitor)
        {
            PlatformSetFloating(value);
        }
    }

    void Window::SetDecorated(bool value)
    {
        DAIS_TRACE("[Window] SetDecorated");

        m_Decorated = value;

        if (!m_Monitor)
        {
            PlatformSetDecorated(value);
        }
    }

    void Window::SetResizable(bool value)
    {
        DAIS_TRACE("[Window] SetResizable");

        m_Resizable = value;

        if (!m_Monitor)
        {
            PlatformSetResizable(value);
        }
    }

    void Window::SetFocusOnShow(bool value)
    {
        DAIS_TRACE("[Window] SetFocusOnShow");

        m_FocusOnShow = value;
    }

    void Window::SetAutoIconify(bool value)
    {
        DAIS_TRACE("[Window] SetAutoIconify");

        m_AutoIconify = value;
    }

    void Window::SetMousePassThrough(bool value)
    {
        DAIS_TRACE("[Window] SetMousePassThrough");

        m_MousePassthrough = value;
        PlatformSetMousePassThrough(value);
    }

    void Window::SetShouldClose(bool value)
    {
        DAIS_TRACE("[Window] SetShouldClose");

        m_ShouldClose = value;
    }

    void Window::SetMonitor(Monitor* monitor, int32_t x, int32_t y, int32_t width, int32_t height, int32_t refreshRate)
    {
        DAIS_TRACE("[Window] SetMonitor");

        if (width <= 0
            || height <= 0)
        {
            DAIS_ERROR("Invalid window size %i x %i!", width, height);
            return;
        }

        if (refreshRate < 0
            && refreshRate != -1)
        {
            DAIS_ERROR("Invalid refresh rate %i!", refreshRate);
            return;
        }

        m_VideoMode.width = width;
        m_VideoMode.height = height;
        m_VideoMode.refreshRate = refreshRate;

        PlatformSetMonitor(monitor, x, y, width, height, refreshRate);
    }


    void Window::Maximize()
    {
        DAIS_TRACE("[Window] SetMaximize");

        if (m_Monitor)
        {
            return;
        }
        PlatformMaximize();
    }

    void Window::Minimize()
    {
        DAIS_TRACE("[Window] SetMinimize");

        PlatformMinimize();
    }

    void Window::Restore()
    {
        DAIS_TRACE("[Window] SetRestore");

        PlatformRestore();
    }

    void Window::Show()
    {
        DAIS_TRACE("[Window] Show");

        if (m_Monitor)
        {
            return;
        }

        PlatformShow();

        if (m_FocusOnShow)
        {
            PlatformFocus();
        }
    }

    void Window::Hide()
    {
        DAIS_TRACE("[Window] Hide");

        if (m_Monitor)
        {
            return;
        }

        PlatformHide();
    }

    void Window::Focus()
    {
        DAIS_TRACE("[Window] Focus");

        PlatformFocus();
    }

    void Window::RequestAttention()
    {
        DAIS_TRACE("[Window] RequestAttention");

        PlatformRequestAttention();
    }
}
