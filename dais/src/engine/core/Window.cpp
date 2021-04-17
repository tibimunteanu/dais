#include "engine/core/Platform.h"

namespace dais
{
    ////////////////////////////////////// STATIC ////////////////////////////////////////

    Window* Window::Create(WindowConfig config, FramebufferConfig fbConfig, Monitor* monitor)
    {
        DAIS_TRACE("[Window] Create");

        if (config.title.empty())
        {
            DAIS_ERROR("Window title cannot be empty!");
            return nullptr;
        }

        if (config.width <= 0
            || config.height <= 0)
        {
            DAIS_ERROR("Invalid window size %i x %i", config.width, config.height);
            return nullptr;
        }

        Window* window = PlatformCreate(config, fbConfig, monitor);

        if (!window)
        {
            return nullptr;
        }

        if (config.mousePassthrough)
        {
            window->PlatformSetMousePassThrough(true);
        }

        if (window->GetMonitor())
        {
            //TODO: center cursor
        }
        else
        {
            if (config.visible)
            {
                window->PlatformShow();
                if (config.focused)
                {
                    window->PlatformFocus();
                }
            }
        }

        return window;
    }



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

    bool Window::IsFramebufferTransparent() const
    {
        return PlatformIsFramebufferTransparent();
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

    void Window::GetFramebufferSize(int32_t* width, int32_t* height) const
    {
        if (width) *width = 0;
        if (height) *height = 0;

        PlatformGetFramebufferSize(width, height);
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

    float Window::GetOpacity()
    {
        return PlatformGetOpacity();
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

    void Window::SetSizeLimits(int32_t minWidth, int32_t minHeight, int32_t maxWidth, int32_t maxHeight)
    {
        if (minWidth != -1
            && minHeight != -1)
        {
            if (minWidth < 0
                || minHeight < 0)
            {
                DAIS_ERROR("Invalid window minimum size %i x %i!", minWidth, minHeight);
                return;
            }
        }

        if (maxWidth != -1
            && maxHeight != -1)
        {
            if (maxWidth < 0
                || maxHeight < 0
                || maxWidth < minWidth
                || maxHeight < minHeight)
            {
                DAIS_ERROR("Invalid window maximum size %i x %i!", maxWidth, maxHeight);
                return;
            }
        }

        m_MinWidth = minWidth;
        m_MinHeight = minHeight;
        m_MaxWidth = maxWidth;
        m_MaxHeight = maxHeight;

        if (m_Monitor || !m_Resizable)
        {
            return;
        }

        PlatformSetSizeLimits(minWidth, minHeight, maxWidth, maxHeight);
    }

    void Window::SetAspectRatio(int32_t numerator, int32_t denominator)
    {
        if (numerator != -1
            && denominator != -1)
        {
            if (numerator <= 0
                || denominator <= 0)
            {
                DAIS_ERROR("Invalid window aspect ration %i:%i!", numerator, denominator);
                return;
            }
        }

        m_Numerator = numerator;
        m_Denominator = denominator;

        if (m_Monitor || !m_Resizable)
        {
            return;
        }

        PlatformSetAspectRatio(numerator, denominator);
    }

    void Window::SetOpacity(float opacity)
    {
        if (opacity != opacity
            || opacity < 0.0f
            || opacity > 1.0f)
        {
            DAIS_ERROR("Invalid window opacity %f", opacity);
            return;
        }

        PlatformSetOpacity(opacity);
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


    void Window::SetPositionCallback(WindowPositionCallback callback)
    {
        m_Callbacks.position = callback;
    }

    void Window::SetSizeCallback(WindowSizeCallback callback)
    {
        m_Callbacks.size = callback;
    }

    void Window::SetCloseCallback(WindowCloseCallback callback)
    {
        m_Callbacks.close = callback;
    }

    void Window::SetRefreshCallback(WindowRefreshCallback callback)
    {
        m_Callbacks.refresh = callback;
    }

    void Window::SetFocusCallback(WindowFocusCallback callback)
    {
        m_Callbacks.focus = callback;
    }

    void Window::SetMinimizeCallback(WindowMinimizeCallback callback)
    {
        m_Callbacks.minimize = callback;
    }

    void Window::SetMaximizeCallback(WindowMaximizeCallback callback)
    {
        m_Callbacks.maximize = callback;
    }

    void Window::SetFramebufferSizeCallback(WindowFramebufferSizeCallback callback)
    {
        m_Callbacks.framebufferSize = callback;
    }

    void Window::SetContentScaleCallback(WindowContentScaleCallback callback)
    {
        m_Callbacks.contentScale = callback;
    }

    void Window::SetKeyCallback(WindowKeyCallback callback)
    {
        m_Callbacks.key = callback;
    }

    void Window::SetCharCallback(WindowCharCallback callback)
    {
        m_Callbacks.character = callback;
    }

    void Window::SetCharModsCallback(WindowCharModsCallback callback)
    {
        m_Callbacks.characterMods = callback;
    }

    void Window::SetMouseButtonCallback(WindowMouseButtonCallback callback)
    {
        m_Callbacks.mouseButton = callback;
    }

    void Window::SetCursorPositionCallback(WindowCursorPositionCallback callback)
    {
        m_Callbacks.cursorPosition = callback;
    }

    void Window::SetCursorEnterCallback(WindowCursorEnterCallback callback)
    {
        m_Callbacks.cursorEnter = callback;
    }

    void Window::SetScrollCallback(WindowScrollCallback callback)
    {
        m_Callbacks.scroll = callback;
    }

    void Window::SetDropCallback(WindowDropCallback callback)
    {
        m_Callbacks.drop = callback;
    }


    void Window::OnPositionChanged(int32_t x, int32_t y)
    {
        if (m_Callbacks.position)
        {
            m_Callbacks.position(this, x, y);
        }
    }

    void Window::OnSizeChanged(int32_t width, int32_t height)
    {
        if (m_Callbacks.size)
        {
            m_Callbacks.size(this, width, height);
        }
    }

    void Window::OnClosed()
    {
        m_ShouldClose = true;

        if (m_Callbacks.close)
        {
            m_Callbacks.close(this);
        }
    }

    void Window::OnNeedUpdate()
    {
        if (m_Callbacks.refresh)
        {
            m_Callbacks.refresh(this);
        }
    }

    void Window::OnFocus(bool focused)
    {
        if (m_Callbacks.focus)
        {
            m_Callbacks.focus(this, focused);
        }

        //TODO:
        //if (!focused)
        //{
        //    for (int32_t key = 0; key <= DAIS_KEY_LAST; key++)
        //    {
        //        if (m_Keys[key] == DAIS_PRESS)
        //        {
        //            const int32_t scancode = PlatformGetKeyScancode(key);
        //            InputKeyCallback(key, scancode, DAIS_RELEASE, 0);
        //        }
        //    }

        //    for (int32_t button = 0; button <= DAIS_MOUSE_BUTTON_LAST; button++)
        //    {
        //        if (m_MouseButtons[button] = DAIS_PRESS)
        //        {
        //            InputMouseButtonCallback(button, DAIS_RELEASE, 0);
        //        }
        //    }
        //}
    }

    void Window::OnMinimize(bool minimized)
    {
        if (m_Callbacks.minimize)
        {
            m_Callbacks.minimize(this, minimized);
        }
    }

    void Window::OnMaximize(bool maximized)
    {
        if (m_Callbacks.maximize)
        {
            m_Callbacks.maximize(this, maximized);
        }
    }

    void Window::OnFramebufferSizeChanged(int32_t width, int32_t height)
    {
        if (m_Callbacks.framebufferSize)
        {
            m_Callbacks.framebufferSize(this, width, height);
        }
    }

    void Window::OnContentScaleChanged(float xScale, float yScale)
    {
        if (m_Callbacks.contentScale)
        {
            m_Callbacks.contentScale(this, xScale, yScale);
        }
    }

    void Window::OnMouseButton(int32_t button, int32_t action, int32_t mods)
    {
        //TODO:
        //if (button < 0
        //    || button > DAIS_MOUSE_BUTTON_LAST)
        //{
        //    return;
        //}

        //if (m_LockKeyMods)
        //{
        //    mods &= ~(DAIS_MOD_CAPS_LOCK | DAIS_MOD_NUM_LOCK);
        //}

        //if (action == DAIS_RELEASE
        //    && m_StickyMouseButtons)
        //{
        //    m_MouseButtons[button] = DAIS_STICK
        //}
        //else
        //{
        //    m_MouseButtons[button] = (char)action;
        //}

        if (m_Callbacks.mouseButton)
        {
            m_Callbacks.mouseButton(this, button, action, mods);
        }
    }

    void Window::OnCursorPositionChanged(double x, double y)
    {
        if (m_VirtualCursorPositionX == x
            && m_VirtualCursorPositionY == y)
        {
            return;
        }

        m_VirtualCursorPositionX = x;
        m_VirtualCursorPositionY = y;

        if (m_Callbacks.cursorPosition)
        {
            m_Callbacks.cursorPosition(this, x, y);
        }
    }

    void Window::OnCursorEnter(bool entered)
    {
        if (m_Callbacks.cursorEnter)
        {
            m_Callbacks.cursorEnter(this, entered);
        }
    }

    void Window::OnScroll(double xOffset, double yOffset)
    {
        if (m_Callbacks.scroll)
        {
            m_Callbacks.scroll(this, xOffset, yOffset);
        }
    }

    void Window::OnKey(int32_t key, int32_t scancode, int32_t action, int32_t mods)
    {
        //TODO:
    }

    void Window::OnChar(uint32_t codepoint, int32_t mods, bool plain)
    {
        //TODO:
    }

    void Window::OnCharMods(uint32_t codepoint, int32_t mods)
    {
        //TODO:
    }

    void Window::OnDrop(uint32_t count, const char** paths)
    {
        if (m_Callbacks.drop)
        {
            m_Callbacks.drop(this, count, paths);
        }
    }
}
