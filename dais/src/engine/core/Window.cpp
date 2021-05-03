#include "engine/core/Platform.h"

namespace dais
{
    ////////////////////////////////////// STATIC ////////////////////////////////////////

    Window* Window::Create(const std::string& title, int32_t width, int32_t height,
                           const WindowConfig* windowConfig, const ContextConfig* contextConfig, const FramebufferConfig* framebufferConfig, Monitor* monitor)
    {
        if (title.empty())
        {
            DAIS_ERROR("Window title cannot be empty!");
            return nullptr;
        }

        if (width <= 0
            || height <= 0)
        {
            DAIS_ERROR("Invalid window size %i x %i", width, height);
            return nullptr;
        }

        if (!contextConfig->IsValid())
        {
            DAIS_ERROR("Context config is not valid!");
            return nullptr;
        }

        Window* window = PlatformCreate(title, width, height, windowConfig, contextConfig, framebufferConfig, monitor);

        if (!window)
        {
            return nullptr;
        }

        if (contextConfig->api != ContextAPI::None)
        {
            if (!Context::RefreshContextAttribs(window, contextConfig))
            {
                delete window;
                return nullptr;
            }
        }

        if (windowConfig->mousePassthrough)
        {
            window->PlatformSetMousePassThrough(true);
        }

        if (window->GetMonitor())
        {
            window->CenterCursorInContentArea();
        }
        else
        {
            if (windowConfig->visible)
            {
                window->PlatformShow();
                if (windowConfig->focused)
                {
                    window->PlatformFocus();
                }
            }
        }

        return window;
    }



    ////////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////

    Window::Window(const std::string& title, int32_t width, int32_t height, 
                   const WindowConfig* windowConfig, const ContextConfig* contextConfig, const FramebufferConfig* framebufferConfig, Monitor* monitor)
    {
        m_Title = title;
        m_Width = width;
        m_Height = height;
        m_Decorated = windowConfig->decorated;
        m_FocusOnShow = windowConfig->focusOnShow;
        m_AutoMinimize = windowConfig->autoMinimize;
        m_Floating = windowConfig->floating;
        m_Resizable = windowConfig->resizable;
        m_MousePassthrough = windowConfig->mousePassthrough;
        m_CursorMode = CursorMode::Normal;
        m_MinWidth = -1;
        m_MinHeight = -1;
        m_MaxWidth = -1;
        m_MaxHeight = -1;
        m_Numerator = -1;
        m_Denominator = -1;

        m_VideoMode = {};
        m_VideoMode.width = width;
        m_VideoMode.height = height;
        m_VideoMode.redBits = framebufferConfig->redBits;
        m_VideoMode.greenBits = framebufferConfig->greenBits;
        m_VideoMode.blueBits = framebufferConfig->blueBits;
        m_VideoMode.refreshRate = Platform::s_Hints.refreshRate;

        m_Monitor = monitor;
    }

    Window::~Window()
    {
        //the window's context must not be current on another thread when the window is destroyed
        if (Platform::s_ContextSlot->Get() == this)
        {
            Context::MakeContextCurrent(nullptr);
        }
    }



    /////////////////////////////////////// PUBLIC API ////////////////////////////////////////

    bool Window::IsMaximized() const
    {
        return PlatformIsMaximized();
    }

    bool Window::IsMinimized() const
    {
        return PlatformIsMinimized();
    }

    bool Window::IsVisible() const
    {
        return PlatformIsVisible();
    }

    bool Window::IsFocused() const
    {
        return PlatformIsFocused();
    }

    bool Window::IsHovered() const
    {
        return PlatformIsHovered();
    }

    bool Window::IsFloating() const
    {
        return m_Floating;
    }

    bool Window::IsDecorated() const
    {
        return m_Decorated;
    }

    bool Window::IsResizable() const
    {
        return m_Resizable;
    }

    bool Window::IsFocusOnShow() const
    {
        return m_FocusOnShow;
    }

    bool Window::IsFramebufferTransparent() const
    {
        return PlatformIsFramebufferTransparent();
    }

    bool Window::IsAutoMinimize() const
    {
        return m_AutoMinimize;
    }

    bool Window::IsMousePassThrough() const
    {
        return m_MousePassthrough;
    }

    bool Window::ShouldClose() const
    {
        return m_ShouldClose;
    }


    const std::string& Window::GetTitle() const
    {
        return m_Title;
    }

    void Window::GetPosition(int32_t* x, int32_t* y) const
    {
        if (x) *x = 0;
        if (y) *y = 0;

        PlatformGetPosition(x, y);
    }

    void Window::GetSize(int32_t* width, int32_t* height)
    {
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
        if (left) *left = 0;
        if (top) *top = 0;
        if (right) *right = 0;
        if (bottom) *bottom = 0;

        PlatformGetFrameSize(left, top, right, bottom);
    }

    void Window::GetContentScale(float* xScale, float* yScale)
    {
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
        return m_Monitor;
    }

    /// <summary> Returns CursorMode if InputMode::Cursor, boolean otherwise </summary>
    int32_t Window::GetInputMode(InputMode mode)
    {
        switch (mode)
        {
            case InputMode::Cursor:             return (int32_t)m_CursorMode;
            case InputMode::StickyKeys:         return m_StickyKeys;
            case InputMode::StickyMouseButtons: return m_StickyMouseButtons;
            case InputMode::LockKeyMods:        return m_LockKeyMods;
            case InputMode::RawMouseMotion:     return m_RawMouseMotion;
        }

        DAIS_ERROR("Invalid input mode %d", (int32_t)mode);
        return false;
    }

    KeyState Window::GetKey(Key key)
    {
        if (m_Keys[(int32_t)key] == KeyState::Stick)
        {
            //sticky mode: release key now
            m_Keys[(int32_t)key] = KeyState::Release;
            return KeyState::Press;
        }

        return m_Keys[(int32_t)key];
    }

    KeyState Window::GetGetMouseButton(MouseButton button)
    {
        if (m_MouseButtons[(int32_t)button] == KeyState::Stick)
        {
            //sticky mode: release mouse button now
            m_MouseButtons[(int32_t)button] = KeyState::Release;
            return KeyState::Press;
        }

        return m_MouseButtons[(int32_t)button];
    }

    void Window::GetCursorPosition(double* x, double* y)
    {
        if (x) *x = 0;
        if (y) *y = 0;

        if (m_CursorMode == CursorMode::Disabled)
        {
            if (x) *x = m_VirtualCursorPositionX;
            if (y) *y = m_VirtualCursorPositionY;
        }
        else
        {
            PlatformGetCursorPosition(x, y);
        }
    }

    Context* Window::GetContext()
    {
        return m_Context;
    }

    void* Window::GetNativeHandle() const
    {
        return PlatformGetHandle();
    }


    void Window::SetTitle(const std::string& title)
    {
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
        if (m_Monitor)
        {
            return;
        }

        PlatformSetPosition(x, y);
    }

    void Window::SetSize(int32_t width, int32_t height)
    {
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
        m_Floating = value;

        if (!m_Monitor)
        {
            PlatformSetFloating(value);
        }
    }

    void Window::SetDecorated(bool value)
    {
        m_Decorated = value;

        if (!m_Monitor)
        {
            PlatformSetDecorated(value);
        }
    }

    void Window::SetResizable(bool value)
    {
        m_Resizable = value;

        if (!m_Monitor)
        {
            PlatformSetResizable(value);
        }
    }

    void Window::SetFocusOnShow(bool value)
    {
        m_FocusOnShow = value;
    }

    void Window::SetAutoMinimize(bool value)
    {
        m_AutoMinimize = value;
    }

    void Window::SetMousePassThrough(bool value)
    {
        m_MousePassthrough = value;
        PlatformSetMousePassThrough(value);
    }

    void Window::SetShouldClose(bool value)
    {
        m_ShouldClose = value;
    }

    void Window::SetMonitor(Monitor* monitor, int32_t x, int32_t y, int32_t width, int32_t height, int32_t refreshRate)
    {
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

    void Window::SetInputMode(InputMode mode, int32_t value)
    {
        if (mode == InputMode::Cursor)
        {
            CursorMode cursorMode = (CursorMode)value;

            if (cursorMode != CursorMode::Normal
                && cursorMode != CursorMode::Hidden
                && cursorMode != CursorMode::Disabled)
            {
                DAIS_ERROR("Invalid cursor mode 0x%08X", value);
                return;
            }

            if (m_CursorMode == cursorMode)
            {
                return;
            }

            m_CursorMode = cursorMode;

            PlatformGetCursorPosition(&m_VirtualCursorPositionX, &m_VirtualCursorPositionY);
            PlatformSetCursorMode(cursorMode);
        }
        else if (mode == InputMode::StickyKeys)
        {
            value = value ? true : false;
            if (m_StickyKeys == value)
            {
                return;
            }

            if (!value)
            {
                //release all sticky keys
                for (int32_t i = 0; i < (int32_t)Key::Count; i++)
                {
                    if (m_Keys[i] == KeyState::Stick)
                    {
                        m_Keys[i] = KeyState::Release;
                    }
                }
            }

            m_StickyKeys = value;
        }
        else if (mode == InputMode::StickyMouseButtons)
        {
            value = value ? true : false;
            if (m_StickyMouseButtons == value)
            {
                return;
            }

            if (!value)
            {
                //release all sticky mouse buttons
                for (int32_t i = 0; i < (int32_t)MouseButton::Count; i++)
                {
                    if (m_MouseButtons[i] == KeyState::Stick)
                    {
                        m_MouseButtons[i] = KeyState::Release;
                    }
                }
            }

            m_StickyMouseButtons = value;
        }
        else if (mode == InputMode::LockKeyMods)
        {
            m_LockKeyMods = value ? true : false;
        }
        else if (mode == InputMode::RawMouseMotion)
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

        if (m_CursorMode == CursorMode::Disabled)
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
        if (m_Monitor)
        {
            return;
        }
        PlatformMaximize();
    }

    void Window::Minimize()
    {
        PlatformMinimize();
    }

    void Window::Restore()
    {
        PlatformRestore();
    }

    void Window::Show()
    {
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
        if (m_Monitor)
        {
            return;
        }

        PlatformHide();
    }

    void Window::Focus()
    {
        PlatformFocus();
    }

    void Window::RequestAttention()
    {
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



    ///////////////////////////////////// EVENT INPUT API /////////////////////////////////////

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
            for (int32_t keyIndex = 0; keyIndex < (int32_t)Key::Count; keyIndex++)
            {
                Key key = (Key)keyIndex;
                if (m_Keys[keyIndex] == KeyState::Press)
                {
                    const int32_t scancode = Platform::GetKeyScancode(key);
                    OnKey(key, scancode, KeyState::Release, KeyMods::None);
                }
            }

            for (int32_t buttonIndex = 0; buttonIndex < (int32_t)MouseButton::Count; buttonIndex++)
            {
                if (m_MouseButtons[buttonIndex] == KeyState::Press)
                {
                    OnMouseButton((MouseButton)buttonIndex, KeyState::Release, KeyMods::None);
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

    void Window::OnMouseButton(MouseButton button, KeyState action, KeyMods mods)
    {
        if (!m_LockKeyMods)
        {
            mods = mods & ~(KeyMods::CapsLock | KeyMods::NumLock);
        }

        if (action == KeyState::Release
            && m_StickyMouseButtons)
        {
            m_MouseButtons[(int32_t)button] = KeyState::Stick;
        }
        else
        {
            m_MouseButtons[(int32_t)button] = action;
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

    void Window::OnKey(Key key, int32_t scancode, KeyState action, KeyMods mods)
    {
        int32_t keyIndex = (int32_t)key;

        if (keyIndex >= 0
            && keyIndex < (int32_t)Key::Count)
        {
            bool repeated = false;

            if (action == KeyState::Release
                && m_Keys[keyIndex] == KeyState::Release)
            {
                return;
            }

            if (action == KeyState::Press
                && m_Keys[keyIndex] == KeyState::Press)
            {
                repeated = true;
            }

            if (action == KeyState::Release
                && m_StickyKeys)
            {
                m_Keys[keyIndex] = KeyState::Stick;
            }
            else
            {
                m_Keys[keyIndex] = action;
            }

            if (repeated)
            {
                action = KeyState::Repeat;
            }
        }

        if (!m_LockKeyMods)
        {
            mods = mods & ~(KeyMods::CapsLock | KeyMods::NumLock);
        }

        if (m_Callbacks.key)
        {
            m_Callbacks.key(this, key, scancode, action, mods);
        }
    }

    void Window::OnChar(uint32_t codepoint, KeyMods mods, bool plain)
    {
        if (codepoint < 32
            || (codepoint > 126 && codepoint < 160))
        {
            return;
        }

        if (!m_LockKeyMods)
        {
            mods = mods & ~(KeyMods::CapsLock | KeyMods::NumLock);
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

    void Window::OnCharMods(uint32_t codepoint, KeyMods mods)
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



    ///////////////////////////////////////// UTILS ///////////////////////////////////////////

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
}
