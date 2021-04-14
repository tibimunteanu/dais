#include "platform/windows/WindowsWindow.h"

namespace dais
{
    Window* Window::Create(WindowConfig config, Monitor* monitor)
    {
        //setup style, position and size
        DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

        if (monitor)
        {
            style |= WS_POPUP; //so it can be created without a border
        }
        else
        {
            style |= WS_SYSMENU | WS_MINIMIZEBOX;

            if (config.decorated)
            {
                style |= WS_CAPTION;

                if (config.resizable)
                {
                    style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
                }
            }
            else
            {
                style |= WS_POPUP;
            }
        }

        DWORD styleEx = WS_EX_APPWINDOW;

        if (monitor
            || config.floating)
        {
            styleEx |= WS_EX_TOPMOST;
        }

        int xPos, yPos;
        int fullWidth, fullHeight;
        if (monitor)
        {
            monitor->GetPosition(&xPos, &yPos);
            VideoMode* videoMode = monitor->GetVideoMode();

            fullWidth = videoMode->Width;
            fullHeight = videoMode->Height;
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

        WCHAR* wideTitle = WindowsBase::UTF8ToWideString(config.title.c_str());

        WindowsWindow* window = new WindowsWindow(config, monitor);

        HWND windowHandle = CreateWindowExW(styleEx,
            DAIS_WINDOW_CLASS,
            wideTitle,
            style,
            xPos, yPos,
            fullWidth, fullHeight,
            nullptr, //no parent window
            nullptr, //no window menu
            GetModuleHandleW(nullptr),
            window);

        delete wideTitle;

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
            WINDOWPLACEMENT wp = {};
            wp.length = sizeof(wp);

            if (config.scaleToMonitor)
            {
                float xScale, yScale;
                window->PlatformGetContentScale(&xScale, &yScale);
                rect.right = (int)(rect.right * xScale);
                rect.bottom = (int)(rect.bottom * yScale);
            }

            ClientToScreen(windowHandle, (POINT*)&rect.left);
            ClientToScreen(windowHandle, (POINT*)&rect.right);

            if (WindowsBase::IsWindows10AnniversaryUpdateOrGreater())
            {
                UINT dpi = WindowsBase::Libs.User32.GetDpiForWindow(windowHandle);
                WindowsBase::Libs.User32.AdjustWindowRectExForDpi(&rect, style, FALSE, styleEx, dpi);
            }
            else
            {
                AdjustWindowRectEx(&rect, style, FALSE, styleEx);
            }

            //only update the restored window rect as the window may be maximizesd
            GetWindowPlacement(windowHandle, &wp);
            wp.rcNormalPosition = rect;
            wp.showCmd = SW_HIDE;
            SetWindowPlacement(windowHandle, &wp);
        }

        window->GetSize(&window->m_Width, &window->m_Height);

        if (window->m_Monitor)
        {
            window->PlatformShow();
            window->PlatformFocus();
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

    WindowsWindow::WindowsWindow(WindowConfig config, Monitor* monitor)
        : Window(config, monitor)
    {
        DAIS_TRACE("[WindowsWindow] Constructor");


    }

    WindowsWindow::~WindowsWindow()
    {
        DAIS_TRACE("[WindowsWindow] Destructor");
    }

    void* WindowsWindow::GetHandle() const
    {
        return nullptr;
    }

    void* WindowsWindow::GetUserData() const
    {
        return nullptr;
    }

    void WindowsWindow::SetUserData(void* data)
    {
    }

    uint32_t WindowsWindow::GetMonitorCount() const
    {
        return 0;
    }

    uint32_t WindowsWindow::GetMonitor() const
    {
        return 0;
    }

    void WindowsWindow::SetMonitor(uint32_t monitor)
    {
    }

    const std::string& WindowsWindow::GetTitle() const
    {
        return m_Title;
    }

    void WindowsWindow::SetTitle(const std::string& title)
    {
    }

    bool WindowsWindow::IsFullscreen() const
    {
        return false;
    }

    void WindowsWindow::SetFullscreen(bool fullscreen)
    {
    }

    void WindowsWindow::GetSize(uint32_t* width, uint32_t* height) const
    {
    }

    void WindowsWindow::SetSize(uint32_t width, uint32_t height)
    {
    }

    void WindowsWindow::GetPosition(uint32_t* x, uint32_t* y) const
    {
    }

    void WindowsWindow::SetPosition(uint32_t x, uint32_t y)
    {
    }

    bool WindowsWindow::IsMaximized() const
    {
        return false;
    }

    void WindowsWindow::Maximize()
    {
    }

    bool WindowsWindow::IsMinimized() const
    {
        return false;
    }

    void WindowsWindow::Minimize()
    {
    }

    void WindowsWindow::Restore()
    {
    }

    bool WindowsWindow::ShouldClose() const
    {
        return false;
    }

    void WindowsWindow::Close()
    {
        DAIS_TRACE("[WindowsWindow] Close");
    }

    LRESULT WindowsWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            default:
            {
                return DefWindowProcW(m_Handle, uMsg, wParam, lParam);
            }
        }
    }


    void WindowsWindow::PlatformShow()
    {
        ShowWindow(m_Handle, SW_SHOWNA);
    }

    void WindowsWindow::PlatformHide()
    {
        ShowWindow(m_Handle, SW_HIDE);
    }

    void WindowsWindow::PlatformFocus()
    {
        BringWindowToTop(m_Handle);
        SetForegroundWindow(m_Handle);
        SetFocus(m_Handle);
    }

    void WindowsWindow::PlatformGetContentScale(float* xScale, float* yScale)
    {
        HMONITOR monitorHandle = MonitorFromWindow(m_Handle, MONITOR_DEFAULTTONEAREST);
        WindowsMonitor::GetContentScale(monitorHandle, xScale, yScale);
    }


    void WindowsWindow::GetFullSize(DWORD style, DWORD styleEx,
        int contentWidth, int contentHeight,
        int* fullWidth, int* fullHeight,
        UINT dpi)
    {
        RECT rect = { 0, 0, contentWidth, contentHeight };

        if (WindowsBase::IsWindows10AnniversaryUpdateOrGreater())
        {
            WindowsBase::Libs.User32.AdjustWindowRectExForDpi(&rect, style, FALSE, styleEx, dpi);
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
            if (WindowsBase::IsWindows10AnniversaryUpdateOrGreater())
            {
                WindowsBase::Libs.User32.EnableNonClientDpiScaling(hwnd);
            }

            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            if (pCreate->lpCreateParams)
            {
                pThis = (WindowsWindow*)pCreate->lpCreateParams;
                pThis->m_Handle = hwnd;

                SetPropW(hwnd, DAIS_WINDOW_PROP, pThis);
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
}

