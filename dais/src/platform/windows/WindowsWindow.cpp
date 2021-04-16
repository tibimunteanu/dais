#include "platform/windows/WindowsPlatform.h"


namespace dais
{
    //////////////////////////////////////// STATIC ///////////////////////////////////////////

        /// <summary> Translate content area size to full window size according to styles and DPI </summary>
    void WindowsWindow::GetFullSize(DWORD style, DWORD styleEx,
        int contentWidth, int contentHeight,
        int* fullWidth, int* fullHeight,
        UINT dpi)
    {
        RECT rect = { 0, 0, contentWidth, contentHeight };

        if (WindowsPlatform::IsWindows10AnniversaryUpdateOrGreater())
        {
            WindowsPlatform::s_Libs.User32.AdjustWindowRectExForDpi(&rect, style, FALSE, styleEx, dpi);
        }
        else
        {
            AdjustWindowRectEx(&rect, style, FALSE, styleEx);
        }

        *fullWidth = rect.right - rect.left;
        *fullHeight = rect.bottom - rect.top;
    }

    /// <summary> static WindowProc which dispatches messages to window instance </summary>
    LRESULT CALLBACK WindowsWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        WindowsWindow* pThis = nullptr;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;

            if (pCreate->lpCreateParams)
            {
                pThis = (WindowsWindow*)pCreate->lpCreateParams;
                pThis->m_Handle = hwnd;

                SetPropW(hwnd, DAIS_WINDOW_PROP, pThis);

                if (WindowsPlatform::IsWindows10AnniversaryUpdateOrGreater())
                {
                    // on per-monitor DPI aware V1 systems, only enable
                    // non-client scaling for windows that scale the client area.
                    // we need WM_GETDPISCALEDSIZE from V2 to keep the client
                    // area static when the non-client area is scaled.
                    if (pThis->m_ScaleToMonitor)
                    {
                        WindowsPlatform::s_Libs.User32.EnableNonClientDpiScaling(hwnd);
                    }
                }
            }
        }
        else
        {
            pThis = (WindowsWindow*)GetPropW(hwnd, DAIS_WINDOW_PROP);
        }

        if (pThis)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            //helper window message handling
            switch (uMsg)
            {
                case WM_DISPLAYCHANGE:
                {
                    //PollMonitors();
                    break;
                }

                case WM_DEVICECHANGE:
                {
                    //if (!joysticksInitialized)
                    //{
                    //    break;
                    //}

                    if (wParam == DBT_DEVICEARRIVAL)
                    {
                        DEV_BROADCAST_HDR* dbh = (DEV_BROADCAST_HDR*)lParam;
                        if (dbh && dbh->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
                        {
                            DAIS_TRACE("DetectJoystickConnection");
                            //DetectJoystickConnection();
                        }
                    }
                    else if (wParam == DBT_DEVICEREMOVECOMPLETE)
                    {
                        DEV_BROADCAST_HDR* dbh = (DEV_BROADCAST_HDR*)lParam;
                        if (dbh && dbh->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
                        {
                            DAIS_TRACE("DetectJoystickDisconnection");
                            //DetectJoystickDisconnection();
                        }
                    }
                    break;
                }

                default:
                {
                    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
                }
            }
        }
    }



    ////////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////

    Window* Window::Create(WindowConfig config, FramebufferConfig fbConfig, Monitor* monitor)
    {
        DAIS_TRACE("[WindowsWindow] Create");

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

        WindowsWindow* window = new WindowsWindow(config, fbConfig, monitor);

        DWORD style = window->GetStyle();
        DWORD styleEx = window->GetStyleEx();

        //setup position
        int xPos, yPos;
        int fullWidth, fullHeight;
        if (monitor)
        {
            //temporary placement until the monitor video mode is set
            monitor->GetPosition(&xPos, &yPos);
            VideoMode* videoMode = monitor->GetVideoMode();

            fullWidth = videoMode->width;
            fullHeight = videoMode->height;
        }
        else
        {
            xPos = CW_USEDEFAULT;
            yPos = CW_USEDEFAULT;

            if (config.maximized)
            {
                style |= WS_MAXIMIZE;
            }

            WindowsWindow::GetFullSize(style, styleEx,
                config.width, config.height,
                &fullWidth, &fullHeight,
                USER_DEFAULT_SCREEN_DPI);
        }

        WCHAR* wideTitle = WindowsPlatform::UTF8ToWideString(config.title.c_str());
        if (!wideTitle)
        {
            DAIS_ERROR("Failed to create window with invalid title '%s'!", config.title.c_str());

            delete window;
            return nullptr;
        }

        HWND windowHandle = CreateWindowExW(styleEx,
            DAIS_WINDOW_CLASS,
            wideTitle,
            style,
            xPos, yPos,
            fullWidth, fullHeight,
            NULL, //no parent window
            NULL, //no window menu
            GetModuleHandleW(NULL),
            window);

        free(wideTitle);

        if (!windowHandle)
        {
            DAIS_ERROR("Failed to create window '%s'!", config.title.c_str());

            delete window;
            return nullptr;
        }

        // adjust window rect to account for DPI scaling of the window frame 
        // and (if enabled) DPI scaling of the content area
        // this cannot be done until we know what monitor the window was placed on
        if (!monitor)
        {
            RECT rect = { 0, 0, config.width, config.height };

            if (config.scaleToMonitor)
            {
                float xScale, yScale;
                window->PlatformGetContentScale(&xScale, &yScale);
                rect.right = (int)(rect.right * xScale);
                rect.bottom = (int)(rect.bottom * yScale);
            }

            ClientToScreen(windowHandle, (POINT*)&rect.left);
            ClientToScreen(windowHandle, (POINT*)&rect.right);

            if (WindowsPlatform::IsWindows10AnniversaryUpdateOrGreater())
            {
                UINT dpi = WindowsPlatform::s_Libs.User32.GetDpiForWindow(windowHandle);
                WindowsPlatform::s_Libs.User32.AdjustWindowRectExForDpi(&rect, style, FALSE, styleEx, dpi);
            }
            else
            {
                AdjustWindowRectEx(&rect, style, FALSE, styleEx);
            }

            //only update the restored window rect as the window may be maximizesd
            WINDOWPLACEMENT wp = {};
            wp.length = sizeof(wp);
            GetWindowPlacement(windowHandle, &wp);
            wp.rcNormalPosition = rect;
            wp.showCmd = SW_HIDE;
            SetWindowPlacement(windowHandle, &wp);
        }

        window->PlatformGetSize(&window->m_Width, &window->m_Height);

        if (window->m_Monitor)
        {
            window->PlatformShow();
            window->PlatformFocus();
            window->AcquireMonitor();
            window->FitToMonitor();
        }
        else if (config.visible)
        {
            window->PlatformShow();
            if (config.focused)
            {
                window->PlatformFocus();
            }
        }

        return window;
    }


    WindowsWindow::WindowsWindow(WindowConfig config, FramebufferConfig fbConfig, Monitor* monitor)
        : Window(config, fbConfig, monitor)
    {
        DAIS_TRACE("[WindowsWindow] Constructor");

        m_Maximized = config.maximized;
        m_ScaleToMonitor = config.scaleToMonitor;
    }

    WindowsWindow::~WindowsWindow()
    {
        DAIS_TRACE("[WindowsWindow] Destructor");

        if (m_Monitor)
        {
            ReleaseMonitor();
        }

        if (m_Handle)
        {
            RemovePropW(m_Handle, DAIS_WINDOW_PROP);
            DestroyWindow(m_Handle);
            m_Handle = nullptr;
        }
    }



    ///////////////////////////////////// PLATFORM API ////////////////////////////////////////

    bool WindowsWindow::PlatformIsMaximized() const
    {
        DAIS_TRACE("[WindowsWindow] PlatformIsMaximized");

        return IsZoomed(m_Handle);
    }

    bool WindowsWindow::PlatformIsMinimized() const
    {
        DAIS_TRACE("[WindowsWindow] PlatformIsMinimized");

        return IsIconic(m_Handle);
    }

    bool WindowsWindow::PlatformIsVisible() const
    {
        DAIS_TRACE("[WindowsWindow] PlatformIsVisible");

        return IsWindowVisible(m_Handle);
    }

    bool WindowsWindow::PlatformIsHovered() const
    {
        DAIS_TRACE("[WindowsWindow] PlatformIsHovered");

        return IsCursorInContentArea();
    }

    bool WindowsWindow::PlatformIsFocused() const
    {
        DAIS_TRACE("[WindowsWindow] PlatformIsFocused");

        return GetActiveWindow() == m_Handle;
    }


    void WindowsWindow::PlatformGetPosition(int32_t* x, int32_t* y) const
    {
        DAIS_TRACE("[WindowsWindow] PlatformGetPosition");

        POINT position = { 0, 0 };
        ClientToScreen(m_Handle, &position);

        if (x)
        {
            *x = position.x;
        }
        if (y)
        {
            *y = position.y;
        }
    }

    void WindowsWindow::PlatformGetSize(int32_t* width, int32_t* height) const
    {
        DAIS_TRACE("[WindowsWindow] PlatformGetSize");

        RECT rect;
        GetClientRect(m_Handle, &rect);

        if (width)
        {
            *width = rect.right;
        }
        if (height)
        {
            *height = rect.bottom;
        }
    }

    void WindowsWindow::PlatformGetFrameSize(int32_t* left, int32_t* top, int32_t* right, int32_t* bottom) const
    {
        DAIS_TRACE("[WindowsWindow] PlatformGetFrameSize");

        RECT rect;
        int32_t width, height;

        PlatformGetSize(&width, &height);
        SetRect(&rect, 0, 0, width, height);

        if (WindowsPlatform::IsWindows10AnniversaryUpdateOrGreater())
        {
            UINT dpi = WindowsPlatform::s_Libs.User32.GetDpiForWindow(m_Handle);
            WindowsPlatform::s_Libs.User32.AdjustWindowRectExForDpi(&rect, GetStyle(), FALSE, GetStyleEx(), dpi);
        }
        else
        {
            AdjustWindowRectEx(&rect, GetStyle(), FALSE, GetStyleEx());
        }

        if (left)
        {
            *left = -rect.left;
        }
        if (top)
        {
            *top = -rect.top;
        }
        if (right)
        {
            *right = rect.right - width;
        }
        if (bottom)
        {
            *bottom = rect.bottom - height;
        }
    }

    void WindowsWindow::PlatformGetContentScale(float* xScale, float* yScale)
    {
        DAIS_TRACE("[WindowsWindow] PlatformGetContentScale");

        HMONITOR monitorHandle = MonitorFromWindow(m_Handle, MONITOR_DEFAULTTONEAREST);
        WindowsMonitor::GetContentScale(monitorHandle, xScale, yScale);
    }

    void* WindowsWindow::PlatformGetHandle() const
    {
        DAIS_TRACE("[WindowsWindow] PlatformGetHandle");

        return m_Handle;
    }


    void WindowsWindow::PlatformSetTitle(const std::string& title)
    {
        DAIS_TRACE("[WindowsWindow] PlatformSetTitle");

        WCHAR* wideTitle = WindowsPlatform::UTF8ToWideString(title.c_str());
        if (!wideTitle)
        {
            return;
        }
        SetWindowTextW(m_Handle, wideTitle);
        free(wideTitle);
    }

    void WindowsWindow::PlatformSetPosition(int32_t x, int32_t y)
    {
        DAIS_TRACE("[WindowsWindow] PlatformSetPosition");

        RECT rect = { x, y, x, y };

        if (WindowsPlatform::IsWindows10AnniversaryUpdateOrGreater())
        {
            UINT dpi = WindowsPlatform::s_Libs.User32.GetDpiForWindow(m_Handle);
            WindowsPlatform::s_Libs.User32.AdjustWindowRectExForDpi(&rect, GetStyle(), FALSE, GetStyleEx(), dpi);
        }
        else
        {
            AdjustWindowRectEx(&rect, GetStyle(), FALSE, GetStyleEx());
        }

        SetWindowPos(m_Handle, NULL,
            rect.left, rect.top, 0, 0,
            SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
    }

    void WindowsWindow::PlatformSetSize(int32_t width, int32_t height)
    {
        DAIS_TRACE("[WindowsWindow] PlatformSetSize");

        if (m_Monitor)
        {
            if (m_Monitor->GetWindow() == this)
            {
                AcquireMonitor();
                FitToMonitor();
            }
        }
        else
        {
            RECT rect = { 0, 0, width, height };

            if (WindowsPlatform::IsWindows10AnniversaryUpdateOrGreater())
            {
                UINT dpi = WindowsPlatform::s_Libs.User32.GetDpiForWindow(m_Handle);
                WindowsPlatform::s_Libs.User32.AdjustWindowRectExForDpi(&rect, GetStyle(), FALSE, GetStyleEx(), dpi);
            }
            else
            {
                AdjustWindowRectEx(&rect, GetStyle(), FALSE, GetStyleEx());
            }

            SetWindowPos(m_Handle, HWND_TOP,
                0, 0, rect.right - rect.left, rect.bottom - rect.top,
                SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
        }
    }

    void WindowsWindow::PlatformSetDecorated(bool value)
    {
        DAIS_TRACE("[WindowsWindow] PlatformSetDecorated");

        UpdateStyles();
    }

    void WindowsWindow::PlatformSetFloating(bool value)
    {
        DAIS_TRACE("[WindowsWindow] PlatformSetFloating");

        const HWND after = value
            ? HWND_TOPMOST
            : HWND_NOTOPMOST;

        SetWindowPos(m_Handle, after, 0, 0, 0, 0,
            SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
    }

    void WindowsWindow::PlatformSetResizable(bool value)
    {
        DAIS_TRACE("[WindowsWindow] PlatformSetResizable");

        UpdateStyles();
    }

    void WindowsWindow::PlatformSetMousePassThrough(bool value)
    {
        DAIS_TRACE("[WindowsWindow] PlatformSetMousePassThrough");

        COLORREF key = 0;
        BYTE alpha = 0;
        DWORD flags = 0;
        DWORD styleEx = GetWindowLongW(m_Handle, GWL_EXSTYLE);

        if (styleEx & WS_EX_LAYERED)
        {
            GetLayeredWindowAttributes(m_Handle, &key, &alpha, &flags);
        }

        if (value)
        {
            styleEx |= (WS_EX_TRANSPARENT | WS_EX_LAYERED);
        }
        else
        {
            styleEx &= ~WS_EX_TRANSPARENT;

            //NOTE: window opacity also needs the layered window style so do not
            //remove it if the window is alpha blended
            if (styleEx & WS_EX_LAYERED)
            {
                if (!(flags & LWA_ALPHA))
                {
                    styleEx &= ~WS_EX_LAYERED;
                }
            }
        }

        SetWindowLongW(m_Handle, GWL_EXSTYLE, styleEx);

        if (value)
        {
            SetLayeredWindowAttributes(m_Handle, key, alpha, flags);
        }
    }

    void WindowsWindow::PlatformSetMonitor(Monitor* monitor, int32_t x, int32_t y, int32_t width, int32_t height, int32_t refreshRate)
    {
        DAIS_TRACE("[WindowsWindow] PlatformSetMonitor");

        if (m_Monitor == monitor)
        {
            if (m_Monitor)
            {
                if (m_Monitor->GetWindow() == this)
                {
                    AcquireMonitor();
                    FitToMonitor();
                }
            }
            else
            {
                RECT rect = { x, y, x + width, y + height };

                if (WindowsPlatform::IsWindows10AnniversaryUpdateOrGreater())
                {
                    UINT dpi = WindowsPlatform::s_Libs.User32.GetDpiForWindow(m_Handle);
                    WindowsPlatform::s_Libs.User32.AdjustWindowRectExForDpi(&rect, GetStyle(), FALSE, GetStyleEx(), dpi);
                }
                else
                {
                    AdjustWindowRectEx(&rect, GetStyle(), FALSE, GetStyleEx());
                }

                SetWindowPos(m_Handle, HWND_TOP,
                    rect.left, rect.top,
                    rect.right - rect.left, rect.bottom - rect.top,
                    SWP_NOCOPYBITS | SWP_NOACTIVATE | SWP_NOZORDER);
            }
            return;
        }
        if (m_Monitor)
        {
            ReleaseMonitor();
        }

        m_Monitor = monitor;

        if (m_Monitor)
        {
            MONITORINFO mi = {};
            mi.cbSize = sizeof(mi);

            UINT flags = SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOCOPYBITS;

            if (m_Decorated)
            {
                DWORD style = GetWindowLongW(m_Handle, GWL_STYLE);
                style &= ~WS_OVERLAPPEDWINDOW;
                style |= GetStyle();
                SetWindowLongW(m_Handle, GWL_STYLE, style);
                flags |= SWP_FRAMECHANGED;
            }

            AcquireMonitor();

            GetMonitorInfo(((WindowsMonitor*)m_Monitor)->m_Handle, &mi);

            SetWindowPos(m_Handle, HWND_TOPMOST,
                mi.rcMonitor.left,
                mi.rcMonitor.top,
                mi.rcMonitor.right - mi.rcMonitor.left,
                mi.rcMonitor.bottom - mi.rcMonitor.top,
                flags);
        }
        else
        {
            RECT rect = { x, y, x + width, y + height };
            DWORD style = GetWindowLongW(m_Handle, GWL_STYLE);
            UINT flags = SWP_NOACTIVATE | SWP_NOCOPYBITS;

            if (m_Decorated)
            {
                style &= ~WS_POPUP;
                style |= GetStyle();
                SetWindowLongW(m_Handle, GWL_STYLE, style);
                flags |= SWP_FRAMECHANGED;
            }

            HWND after = m_Floating
                ? HWND_TOPMOST
                : HWND_NOTOPMOST;

            if (WindowsPlatform::IsWindows10AnniversaryUpdateOrGreater())
            {
                UINT dpi = WindowsPlatform::s_Libs.User32.GetDpiForWindow(m_Handle);
                WindowsPlatform::s_Libs.User32.AdjustWindowRectExForDpi(&rect, GetStyle(), FALSE, GetStyleEx(), dpi);
            }
            else
            {
                AdjustWindowRectEx(&rect, GetStyle(), FALSE, GetStyleEx());
            }

            SetWindowPos(m_Handle, after,
                rect.left, 
                rect.top,
                rect.right - rect.left, 
                rect.bottom - rect.top,
                flags);
        }
    }


    void WindowsWindow::PlatformMaximize()
    {
        DAIS_TRACE("[WindowsWindow] PlatformMaximize");

        ShowWindow(m_Handle, SW_MAXIMIZE);
    }

    void WindowsWindow::PlatformMinimize()
    {
        DAIS_TRACE("[WindowsWindow] PlatformMinimize");

        ShowWindow(m_Handle, SW_MINIMIZE);
    }

    void WindowsWindow::PlatformRestore()
    {
        DAIS_TRACE("[WindowsWindow] PlatformRestore");

        ShowWindow(m_Handle, SW_RESTORE);
    }

    void WindowsWindow::PlatformShow()
    {
        DAIS_TRACE("[WindowsWindow] PlatformShow");

        ShowWindow(m_Handle, SW_SHOWNA);
    }

    void WindowsWindow::PlatformHide()
    {
        DAIS_TRACE("[WindowsWindow] PlatformHide");

        ShowWindow(m_Handle, SW_HIDE);
    }

    void WindowsWindow::PlatformRequestAttention()
    {
        DAIS_TRACE("[WindowsWindow] PlatformRequestAttention");

        FlashWindow(m_Handle, TRUE);
    }

    void WindowsWindow::PlatformFocus()
    {
        DAIS_TRACE("[WindowsWindow] PlatformFocus");

        BringWindowToTop(m_Handle);
        SetForegroundWindow(m_Handle);
        SetFocus(m_Handle);
    }



    ////////////////////////////////////// INTERNAL ///////////////////////////////////////////

    LRESULT WindowsWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            case WM_CLOSE:
            {
                SetShouldClose(true);
                return 0;
            }
            default:
            {
                return DefWindowProcW(m_Handle, uMsg, wParam, lParam);
            }
        }
    }

    /// <summary> Make this window and its video mode active on its monitor </summary>
    void WindowsWindow::AcquireMonitor()
    {
        m_Monitor->SetVideoMode(&m_VideoMode);
        m_Monitor->SetWindow(this);
    }

    /// <summary> Remove the window and restore the original video mode </summary>
    void WindowsWindow::ReleaseMonitor()
    {
        if (m_Monitor->GetWindow() != this)
        {
            return;
        }

        m_Monitor->SetWindow(nullptr);
        m_Monitor->RestoreVideoMode();
    }

    void WindowsWindow::FitToMonitor()
    {
        MONITORINFO mi = {};
        mi.cbSize = sizeof(mi);

        GetMonitorInfo(((WindowsMonitor*)m_Monitor)->m_Handle, &mi);

        SetWindowPos(m_Handle, HWND_TOPMOST,
            mi.rcMonitor.left,
            mi.rcMonitor.top,
            mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS);
    }

    DWORD WindowsWindow::GetStyle() const
    {
        DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

        if (m_Monitor)
        {
            style |= WS_POPUP; //so it can be created without a border
        }
        else
        {
            style |= WS_SYSMENU | WS_MINIMIZEBOX;

            if (m_Decorated)
            {
                style |= WS_CAPTION;

                if (m_Resizable)
                {
                    style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
                }
            }
            else
            {
                style |= WS_POPUP;
            }
        }

        return style;
    }

    DWORD WindowsWindow::GetStyleEx() const
    {
        DWORD styleEx = WS_EX_APPWINDOW;

        if (m_Monitor
            || m_Floating)
        {
            styleEx |= WS_EX_TOPMOST;
        }

        return styleEx;
    }

    void WindowsWindow::UpdateStyles()
    {
        DWORD style = GetWindowLongW(m_Handle, GWL_STYLE);
        style &= !(WS_OVERLAPPEDWINDOW | WS_POPUP);
        style |= GetStyle();

        DWORD styleEx = GetStyleEx();

        RECT rect;
        GetClientRect(m_Handle, &rect);

        if (WindowsPlatform::IsWindows10AnniversaryUpdateOrGreater())
        {
            UINT dpi = WindowsPlatform::s_Libs.User32.GetDpiForWindow(m_Handle);
            WindowsPlatform::s_Libs.User32.AdjustWindowRectExForDpi(&rect, style, FALSE, styleEx, dpi);
        }
        else
        {
            AdjustWindowRectEx(&rect, style, FALSE, styleEx);
        }

        ClientToScreen(m_Handle, (POINT*)&rect.left);
        ClientToScreen(m_Handle, (POINT*)&rect.right);

        SetWindowLongW(m_Handle, GWL_STYLE, style);
        SetWindowPos(m_Handle, HWND_TOP,
            rect.left, rect.top,
            rect.right - rect.left, rect.bottom - rect.top,
            SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOZORDER);
    }

    bool WindowsWindow::IsCursorInContentArea() const
    {
        POINT position;
        if (!GetCursorPos(&position))
        {
            return false;
        }

        if (WindowFromPoint(position) != m_Handle)
        {
            return false;
        }

        RECT rect;
        GetClientRect(m_Handle, &rect);
        ClientToScreen(m_Handle, (POINT*)&rect.left);
        ClientToScreen(m_Handle, (POINT*)&rect.right);

        return PtInRect(&rect, position);
    }
}

