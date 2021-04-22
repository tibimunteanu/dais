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
            window->CenterCursorInContentArea();
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
        m_AutoMinimize = config.autoIconify;
        m_Floating = config.floating;
        m_Resizable = config.resizable;
        m_MousePassthrough = config.mousePassthrough;
        m_CursorMode = DAIS_CURSOR_NORMAL;
        m_MinWidth = -1;
        m_MinHeight = -1;
        m_MaxWidth = -1;
        m_MaxHeight = -1;
        m_Numerator = -1;
        m_Denominator = -1;

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

        return m_AutoMinimize;
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


    /// <summary> DAIS_CURSOR_NORMAL, etc or true false </summary>
    int32_t Window::GetInputMode(int32_t mode)
    {
        switch (mode)
        {
            case DAIS_CURSOR:           return m_CursorMode;
            case DAIS_STICKY_KEYS:      return m_StickyKeys;
            case DAIS_LOCK_KEY_MODS:    return m_StickyMouseButtons;
            case DAIS_RAW_MOUSE_MOTION: return m_RawMouseMotion;
        }

        DAIS_ERROR("Invalid input mode 0x%08X", mode);
        return false;
    }

    int32_t Window::GetKey(int32_t key)
    {
        if (key < DAIS_KEY_SPACE
            || key > DAIS_KEY_LAST)
        {
            DAIS_ERROR("Invalid key %i", key);
            return DAIS_RELEASE;
        }

        if (m_Keys[key] == DAIS_STICK)
        {
            //sticky mode: release key now
            m_Keys[key] = DAIS_RELEASE;
            return DAIS_PRESS;
        }

        return (int32_t)m_Keys[key];
    }

    int32_t Window::GetGetMouseButton(int32_t button)
    {
        if (button < DAIS_MOUSE_BUTTON_1
            || button > DAIS_MOUSE_BUTTON_LAST)
        {
            DAIS_ERROR("Invalid mouse button %i", button);
            return DAIS_RELEASE;
        }

        if (m_MouseButtons[button] = DAIS_STICK)
        {
            //sticky mode: release mouse button now
            m_MouseButtons[button] = DAIS_RELEASE;
            return DAIS_PRESS;
        }

        return (int32_t)m_MouseButtons[button];
    }

    void Window::GetCursorPosition(double* x, double* y)
    {
        if (x) *x = 0;
        if (y) *y = 0;

        if (m_CursorMode == DAIS_CURSOR_DISABLED)
        {
            if (x) *x = m_VirtualCursorPositionX;
            if (y) *y = m_VirtualCursorPositionY;
        }
        else
        {
            PlatformGetCursorPosition(x, y);
        }
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

    void Window::SetIcon(const std::vector<Image*>& images)
    {
        if (!images.size())
        {
            DAIS_ERROR("Parameter 'images' cannot be empty!");
            return;
        }

        PlatformSetIcon(images);
    }

    void Window::SetCursorType(Cursor* cursor)
    {
        m_Cursor = cursor;

        PlatformSetCursorType(cursor);
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

        m_AutoMinimize = value;
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

    void Window::SetInputMode(int32_t mode, int32_t value)
    {
        if (mode == DAIS_CURSOR)
        {
            if (value != DAIS_CURSOR_NORMAL
                && value != DAIS_CURSOR_HIDDEN
                && value != DAIS_CURSOR_DISABLED)
            {
                DAIS_ERROR("Invalid cursor mode 0x%08X", value);
                return;
            }

            if (m_CursorMode == value)
            {
                return;
            }

            m_CursorMode = value;

            PlatformGetCursorPosition(&m_VirtualCursorPositionX, &m_VirtualCursorPositionY);
            PlatformSetCursorMode(value);
        }
        else if (mode == DAIS_STICKY_KEYS)
        {
            value = value ? true : false;
            if (m_StickyKeys == value)
            {
                return;
            }

            if (!value)
            {
                //release all sticky keys
                for (int32_t i = 0; i <= DAIS_KEY_LAST; i++)
                {
                    if (m_Keys[i] == DAIS_STICK)
                    {
                        m_Keys[i] = DAIS_RELEASE;
                    }
                }
            }

            m_StickyKeys = value;
        }
        else if (mode == DAIS_STICKY_MOUSE_BUTTONS)
        {
            value = value ? true : false;
            if (m_StickyMouseButtons == value)
            {
                return;
            }

            if (!value)
            {
                //release all sticky mouse buttons
                for (int32_t i = 0; i <= DAIS_MOUSE_BUTTON_LAST; i++)
                {
                    if (m_MouseButtons[i] == DAIS_STICK)
                    {
                        m_MouseButtons[i] = DAIS_RELEASE;
                    }
                }
            }

            m_StickyMouseButtons = value;
        }
        else if (mode == DAIS_LOCK_KEY_MODS)
        {
            m_LockKeyMods = value ? true : false;
        }
        else if (mode == DAIS_RAW_MOUSE_MOTION)
        {
            if (!Platform::IsRawMouseMotionSupported())
            {
                DAIS_ERROR("Raw mouse motion is not supported on this system!");
                return;
            }

            value = value ? true : false;
            if (m_RawMouseMotion == value)
            {
                return;
            }

            m_RawMouseMotion = value;
            PlatformSetRawMouseMotion(value);
        }
        else
        {
            DAIS_ERROR("Invalid input mode 0x%08X", mode);
        }
    }

    void Window::SetCursorPosition(double x, double y)
    {
        if (x != x
            || x < -DBL_MAX
            || x > DBL_MAX
            || y != y
            || y < -DBL_MAX
            || y > DBL_MAX)
        {
            DAIS_ERROR("Invlid cursor position %f %f", x, y);
            return;
        }

        if (!PlatformIsFocused())
        {
            return;
        }

        if (m_CursorMode == DAIS_CURSOR_DISABLED)
        {
            //onlyy update the accumulated position if the cursor is disabled
            m_VirtualCursorPositionX = x;
            m_VirtualCursorPositionY = y;
        }
        else
        {
            //update system cursor position
            PlatformSetCursorPosition(x, y);
        }
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

    void Window::CenterCursorInContentArea()
    {
        int32_t width, height;
        PlatformGetSize(&width, &height);
        PlatformSetCursorPosition(width * 0.5f, height * 0.5f);
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


    const Image* Window::ChooseImage(const std::vector<Image*>& images, int32_t width, int32_t height)
    {
        int32_t leastDiff = INT_MAX;
        const Image* closest = NULL;

        for (Image* image : images)
        {
            const int32_t currDiff = abs(image->width * image->height - width * height);
            if (currDiff < leastDiff)
            {
                closest = image;
                leastDiff = currDiff;
            }
        }

        return closest;
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

        if (!focused)
        {
            for (int32_t key = 0; key <= DAIS_KEY_LAST; key++)
            {
                if (m_Keys[key] == DAIS_PRESS)
                {
                    const int32_t scancode = Platform::GetKeyScancode(key);
                    OnKey(key, scancode, DAIS_RELEASE, 0);
                }
            }

            for (int32_t button = 0; button <= DAIS_MOUSE_BUTTON_LAST; button++)
            {
                if (m_MouseButtons[button] = DAIS_PRESS)
                {
                    OnMouseButton(button, DAIS_RELEASE, 0);
                }
            }
        }
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
        if (button < 0
            || button > DAIS_MOUSE_BUTTON_LAST)
        {
            return;
        }

        if (!m_LockKeyMods)
        {
            mods &= ~(DAIS_MOD_CAPS_LOCK | DAIS_MOD_NUM_LOCK);
        }

        if (action == DAIS_RELEASE
            && m_StickyMouseButtons)
        {
            m_MouseButtons[button] = DAIS_STICK;
        }
        else
        {
            m_MouseButtons[button] = (int8_t)action;
        }

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
        if (key >= 0
            && key <= DAIS_KEY_LAST)
        {
            bool repeated = false;

            if (action == DAIS_RELEASE
                && m_Keys[key] == DAIS_RELEASE)
            {
                return;
            }

            if (action == DAIS_PRESS
                && m_Keys[key] == DAIS_PRESS)
            {
                repeated = true;
            }

            if (action == DAIS_RELEASE
                && m_StickyKeys)
            {
                m_Keys[key] = DAIS_STICK;
            }
            else
            {
                m_Keys[key] = (int8_t)action;
            }

            if (repeated)
            {
                action = DAIS_REPEAT;
            }
        }

        if (!m_LockKeyMods)
        {
            mods &= ~(DAIS_MOD_CAPS_LOCK | DAIS_MOD_NUM_LOCK);
        }

        if (m_Callbacks.key)
        {
            m_Callbacks.key(this, key, scancode, action, mods);
        }
    }

    void Window::OnChar(uint32_t codepoint, int32_t mods, bool plain)
    {
        if (codepoint < 32
            || (codepoint > 126 && codepoint < 160))
        {
            return;
        }

        if (!m_LockKeyMods)
        {
            mods &= ~(DAIS_MOD_CAPS_LOCK | DAIS_MOD_NUM_LOCK);
        }

        OnCharMods(codepoint, mods);

        if (plain)
        {
            if (m_Callbacks.character)
            {
                m_Callbacks.character(this, codepoint);
            }
        }
    }

    void Window::OnCharMods(uint32_t codepoint, int32_t mods)
    {
        if (m_Callbacks.characterMods)
        {
            m_Callbacks.characterMods(this, codepoint, mods);
        }
    }

    void Window::OnDrop(uint32_t count, const char** paths)
    {
        if (m_Callbacks.drop)
        {
            m_Callbacks.drop(this, count, paths);
        }
    }
}
