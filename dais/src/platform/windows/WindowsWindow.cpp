#include "platform/windows/WindowsPlatform.h"


namespace dais
{
    //////////////////////////////////////// STATIC ///////////////////////////////////////////

    Window* Window::PlatformCreate(WindowConfig config, FramebufferConfig fbConfig, Monitor* monitor)
    {
        DAIS_TRACE("[WindowsWindow] PlatformCreate");

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

        window->ChangeMessageFilter();

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

            WindowsPlatform::AdjustRect(&rect, windowHandle, style, styleEx);

            //only update the restored window rect as the window may be maximizesd
            WINDOWPLACEMENT wp = {};
            wp.length = sizeof(wp);
            GetWindowPlacement(windowHandle, &wp);
            wp.rcNormalPosition = rect;
            wp.showCmd = SW_HIDE;
            SetWindowPlacement(windowHandle, &wp);
        }

        DragAcceptFiles(windowHandle, TRUE);

        if (fbConfig.transparent)
        {
            window->UpdateFramebufferTransparency();
            window->m_Transparent = true;
        }

        window->PlatformGetSize(&window->m_Width, &window->m_Height);

        if (window->m_Monitor)
        {
            window->PlatformShow();
            window->PlatformFocus();
            window->AcquireMonitor();
            window->FitToMonitor();
        }

        return window;
    }

    /// <summary> Translate content area size to full window size according to styles and DPI </summary>
    void WindowsWindow::GetFullSize(DWORD style, DWORD styleEx, int contentWidth, int contentHeight, int* fullWidth, int* fullHeight, UINT dpi)
    {
        RECT rect = { 0, 0, contentWidth, contentHeight };

        WindowsPlatform::AdjustRect(&rect, style, styleEx, dpi);

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
                        WindowsPlatform::s_Libs.user32.EnableNonClientDpiScaling(hwnd);
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
                    WindowsPlatform::PollMonitors();
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

    HICON WindowsWindow::CreateIcon(const Image* image, int32_t xHot, int32_t yHot, bool icon)
    {
        BITMAPV5HEADER bi = {};
        bi.bV5Size = sizeof(bi);
        bi.bV5Width = image->width;
        bi.bV5Height = -image->height;
        bi.bV5Planes = 1;
        bi.bV5BitCount = 32;
        bi.bV5Compression = BI_BITFIELDS;
        bi.bV5RedMask = 0x00ff0000;
        bi.bV5GreenMask = 0x0000ff00;
        bi.bV5BlueMask = 0x000000ff;
        bi.bV5AlphaMask = 0xff000000;

        HDC dc = GetDC(NULL);
        uint8_t* target = NULL;
        HBITMAP color = CreateDIBSection(dc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&target, NULL, (DWORD)0);
        ReleaseDC(NULL, dc);

        if (!color)
        {
            DAIS_ERROR("Failed to create a RGBA bitmap!");
            return NULL;
        }

        HBITMAP mask = CreateBitmap(image->width, image->height, 1, 1, NULL);
        if (!mask)
        {
            DAIS_ERROR("Failed to create a mask bitmap!");
            return NULL;
        }

        uint8_t* source = image->pixels;
        for (int32_t i = 0; i < image->width * image->height; i++)
        {
            target[0] = source[2];
            target[1] = source[1];
            target[2] = source[0];
            target[3] = source[3];
            target += 4;
            source += 4;
        }

        ICONINFO ii = {};
        ii.fIcon = icon;
        ii.xHotspot = xHot;
        ii.yHotspot = yHot;
        ii.hbmMask = mask;
        ii.hbmColor = color;

        HICON handle = CreateIconIndirect(&ii);

        DeleteObject(color);
        DeleteObject(mask);

        if (!handle)
        {
            if (icon)
            {
                DAIS_ERROR("Failed to create icon!");
            }
            else
            {
                DAIS_ERROR("Failed to create cursor!");
            }
        }

        return handle;
    }



    ////////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////

    WindowsWindow::WindowsWindow(WindowConfig config, FramebufferConfig fbConfig, Monitor* monitor)
        : Window(config, fbConfig, monitor)
    {
        DAIS_TRACE("[WindowsWindow] Constructor");

        m_Maximized = config.maximized;
        m_ScaleToMonitor = config.scaleToMonitor;
        m_KeyMenu = config.keyMenu;
    }

    WindowsWindow::~WindowsWindow()
    {
        DAIS_TRACE("[WindowsWindow] Destructor");

        if (m_Monitor)
        {
            ReleaseMonitor();
        }

        if (WindowsPlatform::s_DisabledCursorWindow == this)
        {
            WindowsPlatform::s_DisabledCursorWindow = nullptr;
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

    bool WindowsWindow::PlatformIsFramebufferTransparent() const
    {
        if (!m_Transparent)
        {
            return false;
        }

        if (!WindowsPlatform::IsWindowsVistaOrGreater())
        {
            return false;
        }

        BOOL composition;
        if (FAILED(WindowsPlatform::s_Libs.dwmapi.IsCompositionEnabled(&composition)) || !composition)
        {
            return false;
        }

        if (!WindowsPlatform::IsWindows8OrGreater())
        {
            // HACK: disable framebuffer transparency on Windows 7 when the
            // colorization color is opaque, because otherwise the window
            // contents is blended additively with the previous frame instead
            // of replacing it
            BOOL opaque;
            DWORD color;
            if (FAILED(WindowsPlatform::s_Libs.dwmapi.GetColorizationColor(&color, &opaque)) || opaque)
            {
                return false;
            }
        }

        return true;
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

    void WindowsWindow::PlatformGetFramebufferSize(int32_t* width, int32_t* height) const
    {
        PlatformGetSize(width, height);
    }

    void WindowsWindow::PlatformGetFrameSize(int32_t* left, int32_t* top, int32_t* right, int32_t* bottom) const
    {
        DAIS_TRACE("[WindowsWindow] PlatformGetFrameSize");

        RECT rect;
        int32_t width, height;

        PlatformGetSize(&width, &height);
        SetRect(&rect, 0, 0, width, height);

        AdjustRect(&rect);

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

    void WindowsWindow::PlatformGetCursorPosition(double* x, double* y)
    {
        POINT pos;

        if (GetCursorPos(&pos))
        {
            ScreenToClient(m_Handle, &pos);
        }

        if (x) *x = pos.x;
        if (y) *y = pos.y;
    }

    float WindowsWindow::PlatformGetOpacity()
    {
        BYTE alpha;
        DWORD flags;

        if ((GetWindowLongW(m_Handle, GWL_EXSTYLE) & WS_EX_LAYERED)
            && GetLayeredWindowAttributes(m_Handle, NULL, &alpha, &flags))
        {
            if (flags & LWA_ALPHA)
            {
                return alpha / 255.0f;
            }
        }

        return 1.0f;
    }

    const char* WindowsWindow::PlatformGetClipboardString()
    {
        if (!OpenClipboard(WindowsPlatform::s_HelperWindowHandle))
        {
            DAIS_ERROR("Failed to open clipboard!");
            return nullptr;
        }

        HANDLE object = GetClipboardData(CF_UNICODETEXT);
        if (!object)
        {
            DAIS_ERROR("Failed to convert clipboard to string!");
            CloseClipboard();
            return nullptr;
        }

        WCHAR* buffer = (WCHAR*)GlobalLock(object);
        if (!buffer)
        {
            DAIS_ERROR("Failed to lock global handle!");
            CloseClipboard();
            return nullptr;
        }

        free(WindowsPlatform::s_ClipboardString);
        WindowsPlatform::s_ClipboardString = WindowsPlatform::WideStringToUTF8(buffer);

        GlobalUnlock(object);
        CloseClipboard();

        return WindowsPlatform::s_ClipboardString;
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

    void WindowsWindow::PlatformSetIcon(const std::vector<Image*>& images)
    {
        HICON bigIcon = NULL, smallIcon = NULL;

        if (images.size())
        {
            const Image* bigImage = WindowsWindow::ChooseImage(images, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
            const Image* smallImage = WindowsWindow::ChooseImage(images, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));

            bigIcon = CreateIcon(bigImage, 0, 0, true);
            smallIcon = CreateIcon(smallImage, 0, 0, true);
        }
        else
        {
            bigIcon = (HICON)GetClassLongPtrW(m_Handle, GCLP_HICON);
            smallIcon = (HICON)GetClassLongPtrW(m_Handle, GCLP_HICONSM);
        }

        SendMessageW(m_Handle, WM_SETICON, ICON_BIG, (LPARAM)bigIcon);
        SendMessageW(m_Handle, WM_SETICON, ICON_SMALL, (LPARAM)smallIcon);

        if (m_BigIcon)
        {
            DestroyIcon(m_BigIcon);
        }

        if (m_SmallIcon)
        {
            DestroyIcon(m_SmallIcon);
        }

        if (images.size())
        {
            m_BigIcon = bigIcon;
            m_SmallIcon = smallIcon;
        }
    }

    void WindowsWindow::PlatformSetCursorType(Cursor* cursor)
    {
        if (IsCursorInContentArea())
        {
            UpdateCursorImage();
        }
    }

    void WindowsWindow::PlatformSetPosition(int32_t x, int32_t y)
    {
        DAIS_TRACE("[WindowsWindow] PlatformSetPosition");

        RECT rect = { x, y, x, y };

        AdjustRect(&rect);

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

            AdjustRect(&rect);

            SetWindowPos(m_Handle, HWND_TOP,
                0, 0, rect.right - rect.left, rect.bottom - rect.top,
                SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
        }
    }

    void WindowsWindow::PlatformSetSizeLimits(int32_t minWidth, int32_t minHeight, int32_t maxWidth, int32_t maxHeight)
    {
        if (minWidth == -1
            || minHeight == -1
            || maxWidth == -1
            || maxHeight == -1)
        {
            return;
        }

        RECT rect;
        GetWindowRect(m_Handle, &rect);
        MoveWindow(m_Handle,
            rect.left, rect.top,
            rect.right - rect.left,
            rect.bottom - rect.top,
            TRUE);
    }

    void WindowsWindow::PlatformSetAspectRatio(int32_t numerator, int32_t denominator)
    {
        if (numerator == -1 || denominator == -1)
        {
            return;
        }

        RECT rect;
        GetWindowRect(m_Handle, &rect);
        ApplyAspectRatio(WMSZ_BOTTOMRIGHT, &rect);
        MoveWindow(m_Handle,
            rect.left, rect.top,
            rect.right - rect.left,
            rect.bottom - rect.top,
            TRUE);
    }

    void WindowsWindow::PlatformSetOpacity(float opacity)
    {
        LONG styleEx = GetWindowLongW(m_Handle, GWL_EXSTYLE);
        if (opacity < 1.0f
            || (styleEx & WS_EX_TRANSPARENT))
        {
            const BYTE alpha = (BYTE)(255 * opacity);
            styleEx |= WS_EX_LAYERED;
            SetWindowLongW(m_Handle, GWL_EXSTYLE, styleEx);
            SetLayeredWindowAttributes(m_Handle, 0, alpha, LWA_ALPHA);
        }
        else if (styleEx & WS_EX_TRANSPARENT)
        {
            SetLayeredWindowAttributes(m_Handle, 0, 0, 0);
        }
        else
        {
            styleEx &= ~WS_EX_LAYERED;
            SetWindowLongW(m_Handle, GWL_EXSTYLE, styleEx);
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

    void WindowsWindow::PlatformSetClipboardString(const char* string)
    {
        int characterCount = MultiByteToWideChar(CP_UTF8, 0, string, -1, NULL, 0);
        if (!characterCount)
        {
            return;
        }

        HANDLE object = GlobalAlloc(GMEM_MOVEABLE, characterCount * sizeof(WCHAR));
        if (!object)
        {
            DAIS_ERROR("Failed to allocate global handle for clipboard!");
            return;
        }

        WCHAR* buffer = (WCHAR*)GlobalLock(object);
        if (!buffer)
        {
            DAIS_ERROR("Failed to lock global handle!");
            GlobalFree(object);
            return;
        }

        MultiByteToWideChar(CP_UTF8, 0, string, -1, buffer, characterCount);
        GlobalUnlock(object);

        if (!OpenClipboard(WindowsPlatform::s_HelperWindowHandle))
        {
            DAIS_ERROR("Failed to open clipboard!");
            GlobalFree(object);
            return;
        }

        EmptyClipboard();
        SetClipboardData(CF_UNICODETEXT, object);
        CloseClipboard();
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

                AdjustRect(&rect);

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

            AdjustRect(&rect);

            SetWindowPos(m_Handle, after,
                rect.left,
                rect.top,
                rect.right - rect.left,
                rect.bottom - rect.top,
                flags);
        }
    }

    void WindowsWindow::PlatformSetCursor(Cursor* cursor)
    {
        if (IsCursorInContentArea())
        {
            UpdateCursorImage();
        }
    }

    void WindowsWindow::PlatformSetCursorPosition(double x, double y)
    {
        POINT pos = { (int)x, (int)y };

        //store the new position so it can be recognized later
        m_LastCursorPositionX = pos.x;
        m_LastCursorPositionY = pos.y;

        ClientToScreen(m_Handle, &pos);
        SetCursorPos(pos.x, pos.y);
    }

    void WindowsWindow::PlatformSetCursorMode(int32_t mode)
    {
        if (mode == DAIS_CURSOR_DISABLED)
        {
            if (PlatformIsFocused())
            {
                SetCursorEnabled(false);
            }
        }
        else if (WindowsPlatform::s_DisabledCursorWindow == this)
        {
            SetCursorEnabled(true);
        }
        else if (IsCursorInContentArea())
        {
            UpdateCursorImage();
        }
    }

    /// <summary> enables / disables WM_INPUT messages for the mouse for this window </summary>
    void WindowsWindow::PlatformSetRawMouseMotion(bool enabled)
    {
        if (WindowsPlatform::s_DisabledCursorWindow != this)
        {
            return;
        }

        if (enabled)
        {
            const RAWINPUTDEVICE rid = { 0x01, 0x02, 0, m_Handle };

            if (!RegisterRawInputDevices(&rid, 1, sizeof(rid)))
            {
                DAIS_ERROR("Failed to register raw input device!");
            }
        }
        else
        {
            const RAWINPUTDEVICE rid = { 0x01, 0x02, RIDEV_REMOVE, NULL };

            if (!RegisterRawInputDevices(&rid, 1, sizeof(rid)))
            {
                DAIS_ERROR("Failed to remove raw input device!");
            }
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
            case WM_MOUSEACTIVATE:
            {
                // HACK: postpone cursor disabling when the window was activated
                // by clicking a caption button
                if (HIWORD(lParam) == WM_LBUTTONDOWN)
                {
                    if (LOWORD(lParam) != HTCLIENT)
                    {
                        m_FrameAction = true;
                    }
                }

                break;
            }

            case WM_CAPTURECHANGED:
            {
                // HACK: disable the cursor once the caption button action
                // has been completed or cancelled
                if (lParam == 0
                    && m_FrameAction)
                {
                    if (m_CursorMode == DAIS_CURSOR_DISABLED)
                    {
                        SetCursorEnabled(false);
                    }

                    m_FrameAction = false;
                }

                break;
            }

            case WM_SETFOCUS:
            {
                OnFocus(true);

                // HACK: do not disable cursor while the user is interacting
                // with a caption button
                if (m_FrameAction)
                {
                    break;
                }

                if (m_CursorMode == DAIS_CURSOR_DISABLED)
                {
                    SetCursorEnabled(false);
                }

                return 0;
            }

            case WM_KILLFOCUS:
            {
                if (m_CursorMode == DAIS_CURSOR_DISABLED)
                {
                    SetCursorEnabled(false);
                }

                if (m_Monitor
                    && m_AutoMinimize)
                {
                    PlatformMinimize();
                }

                OnFocus(false);

                return 0;
            }

            case WM_SYSCOMMAND:
            {
                switch (wParam & 0xfff0)
                {
                    case SC_SCREENSAVE:
                    case SC_MONITORPOWER:
                    {
                        if (m_Monitor)
                        {
                            //we are running in full screen mode,
                            //so disallow screen saver and blanking
                            return 0;
                        }
                        else
                        {
                            break;
                        }
                    }

                    //user trying to access application menu using ALT?
                    case SC_KEYMENU:
                    {
                        if (!m_KeyMenu)
                        {
                            return 0;
                        }
                        break;
                    }
                }

                break;
            }

            case WM_CLOSE:
            {
                OnClosed();

                return 0;
            }

            case WM_INPUTLANGCHANGE:
            {
                WindowsPlatform::UpdateKeyNames();
                break;
            }

            case WM_CHAR:
            case WM_SYSCHAR:
            {
                if (wParam >= 0xd800
                    && wParam <= 0xdbff)
                {
                    m_HighSurrogate = (WCHAR)wParam;
                }
                else
                {
                    uint32_t codepoint = 0;

                    if (wParam >= 0xdc00
                        && wParam <= 0xdfff)
                    {
                        if (m_HighSurrogate)
                        {
                            codepoint += (m_HighSurrogate - 0xd800) << 10;
                            codepoint += (WCHAR)wParam - 0xdc00;
                            codepoint += 0x10000;
                        }
                    }
                    else
                    {
                        codepoint = (WCHAR)wParam;
                    }

                    m_HighSurrogate = 0;
                    OnChar(codepoint, GetKeyMods(), uMsg != WM_SYSCHAR);
                }

                if (uMsg == WM_SYSCHAR
                    && m_KeyMenu)
                {
                    break;
                }

                return 0;
            }

            case WM_UNICHAR:
            {
                if (wParam == UNICODE_NOCHAR)
                {
                    //WM_UNICHAR is not sent by Windows, but is sent by some third-party input method engines.
                    //returning TRUE here announces support for this message
                    return TRUE;
                }

                OnChar((uint32_t)wParam, GetKeyMods(), true);

                return 0;
            }

            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYUP:
            {
                const int32_t action = (HIWORD(lParam) & KF_UP) ? DAIS_RELEASE : DAIS_PRESS;
                const int32_t mods = GetKeyMods();

                int32_t scancode = (HIWORD(lParam) & (KF_EXTENDED | 0xff));
                if (!scancode)
                {
                    //NOTE: some synthetic key messages have a scancode of zero
                    //HACK: map the virtual key back to a usable scancode
                    scancode = MapVirtualKeyW((UINT)wParam, MAPVK_VK_TO_VSC);
                }

                int32_t key = WindowsPlatform::s_Keycodes[scancode];

                //the ctrl keys require special handling
                if (wParam == VK_CONTROL)
                {
                    if (HIWORD(lParam) & KF_EXTENDED)
                    {
                        //right side keys have the extended key bit set
                        key = DAIS_KEY_RIGHT_CONTROL;
                    }
                    else
                    {
                        //NOTE: alt gr sends left ctrl followed by right alt
                        //HACK: we only want one event for alt gr, so if we
                        //detect this sequence, we discard this left ctrl
                        //message now and later report right alt normally
                        const DWORD time = GetMessageTime();
                        MSG next;
                        if (PeekMessageW(&next, NULL, 0, 0, PM_NOREMOVE))
                        {
                            if (next.message == WM_KEYDOWN
                                || next.message == WM_SYSKEYDOWN
                                || next.message == WM_KEYUP
                                || next.message == WM_SYSKEYUP)
                            {
                                if (next.wParam == VK_MENU
                                    && (HIWORD(next.lParam) & KF_EXTENDED)
                                    && next.time == time)
                                {
                                    //next message is right alt down so discard this
                                    break;
                                }
                            }
                        }

                        //this is a regular left ctrl message
                        key = DAIS_KEY_LEFT_CONTROL;
                    }
                }
                else if (wParam == VK_PROCESSKEY)
                {
                    //IME notifies that keys have been filteres by setting the
                    //virtual keycode to VK_PROCESSKEY
                    break;
                }

                if (action == DAIS_RELEASE
                    && wParam == VK_SHIFT)
                {
                    //HACK: release both shift keys on shift up event, as when
                    //both are pressed the first release does not exit any event
                    //NOTE: the other half of this is in Platform::PollEvents
                    OnKey(DAIS_KEY_LEFT_SHIFT, scancode, action, mods);
                    OnKey(DAIS_KEY_RIGHT_SHIFT, scancode, action, mods);
                }
                else if (wParam == VK_SNAPSHOT)
                {
                    //HACK: key down is not reported for the print screen key
                    OnKey(key, scancode, DAIS_PRESS, mods);
                    OnKey(key, scancode, DAIS_RELEASE, mods);
                }
                else
                {
                    OnKey(key, scancode, action, mods);
                }

                break;
            }

            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_XBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
            case WM_XBUTTONUP:
            {
                int32_t button;
                if (uMsg == WM_LBUTTONDOWN
                    || uMsg == WM_LBUTTONUP)
                {
                    button = DAIS_MOUSE_BUTTON_LEFT;
                }
                else if (uMsg == WM_RBUTTONDOWN
                    || uMsg == WM_RBUTTONUP)
                {
                    button = DAIS_MOUSE_BUTTON_RIGHT;
                }
                else if (uMsg == WM_MBUTTONDOWN
                    || uMsg == WM_MBUTTONUP)
                {
                    button = DAIS_MOUSE_BUTTON_MIDDLE;
                }
                else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
                {
                    button = DAIS_MOUSE_BUTTON_4;
                }
                else
                {
                    button = DAIS_MOUSE_BUTTON_5;
                }

                int32_t action;
                if (uMsg == WM_LBUTTONDOWN
                    || uMsg == WM_RBUTTONDOWN
                    || uMsg == WM_MBUTTONDOWN
                    || uMsg == WM_XBUTTONDOWN)
                {
                    action = DAIS_PRESS;
                }
                else
                {
                    action = DAIS_RELEASE;
                }

                int32_t i;
                for (i = 0; i <= DAIS_MOUSE_BUTTON_LAST; i++)
                {
                    if (m_MouseButtons[i] == DAIS_PRESS)
                    {
                        break;
                    }
                }

                if (i > DAIS_MOUSE_BUTTON_LAST)
                {
                    ReleaseCapture();
                }

                if (uMsg == WM_XBUTTONDOWN
                    || uMsg == WM_XBUTTONUP)
                {
                    return TRUE;
                }

                return 0;
            }

            case WM_MOUSEMOVE:
            {
                const int x = GET_X_LPARAM(lParam);
                const int y = GET_Y_LPARAM(lParam);

                if (m_CursorTracked)
                {
                    TRACKMOUSEEVENT tme = {};
                    tme.cbSize = sizeof(tme);
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = m_Handle;
                    TrackMouseEvent(&tme);

                    m_CursorTracked = true;

                    OnCursorEnter(true);
                }

                if (m_CursorMode == DAIS_CURSOR_DISABLED)
                {
                    const int dx = x - m_LastCursorPositionX;
                    const int dy = y - m_LastCursorPositionY;

                    if (WindowsPlatform::s_DisabledCursorWindow != this)
                    {
                        break;
                    }

                    if (m_RawMouseMotion)
                    {
                        break;
                    }

                    OnCursorPositionChanged(m_VirtualCursorPositionX + dx, m_VirtualCursorPositionY + dy);
                }
                else
                {
                    OnCursorPositionChanged(x, y);
                }

                m_LastCursorPositionX = x;
                m_LastCursorPositionY = y;

                return 0;
            }

            case WM_INPUT:
            {
                if (WindowsPlatform::s_DisabledCursorWindow != this)
                {
                    break;
                }

                if (!m_RawMouseMotion)
                {
                    break;
                }

                UINT size = 0;
                HRAWINPUT ri = (HRAWINPUT)lParam;

                GetRawInputData(ri, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
                if (size > (UINT)WindowsPlatform::s_RawInputSize)
                {
                    free(WindowsPlatform::s_RawInput);
                    WindowsPlatform::s_RawInput = (RAWINPUT*)calloc(size, 1);
                    WindowsPlatform::s_RawInputSize = size;
                }

                size = WindowsPlatform::s_RawInputSize;

                if (GetRawInputData(ri, RID_INPUT, 
                    WindowsPlatform::s_RawInput, 
                    &size, sizeof(RAWINPUTHEADER)) == (UINT)-1)
                {
                    DAIS_ERROR("Failed to retrieve raw input data!");
                    break;
                }

                RAWINPUT* data = WindowsPlatform::s_RawInput;
                int32_t dx, dy;
                if (data->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
                {
                    dx = data->data.mouse.lLastX - m_LastCursorPositionX;
                    dy = data->data.mouse.lLastY - m_LastCursorPositionY;
                }
                else
                {
                    dx = data->data.mouse.lLastX;
                    dy = data->data.mouse.lLastY;
                }

                OnCursorPositionChanged(m_VirtualCursorPositionX + dx, m_VirtualCursorPositionY + dy);

                m_LastCursorPositionX += dx;
                m_LastCursorPositionY += dy;
                break;
            }

            case WM_MOUSELEAVE:
            {
                m_CursorTracked = false;
                OnCursorEnter(false);

                return 0;
            }

            case WM_MOUSEWHEEL:
            {
                OnScroll(0.0, (SHORT)HIWORD(wParam) / (double)WHEEL_DELTA);

                return 0;
            }

            case WM_MOUSEHWHEEL:
            {
                //this message is only sent on Windows Vista and later
                //NOTE: the x-axis is inverted for consistency with macOS and X11
                OnScroll(-((SHORT)HIWORD(wParam) / (double)WHEEL_DELTA), 0.0);

                return 0;
            }

            case WM_ENTERSIZEMOVE:
            case WM_ENTERMENULOOP:
            {
                if (m_FrameAction)
                {
                    break;
                }

                // HACK: enable the cursor while the user is moving
                // or resizing the window or using the window menu
                if (m_CursorMode == DAIS_CURSOR_DISABLED)
                {
                    SetCursorEnabled(true);
                }

                break;
            }

            case WM_EXITSIZEMOVE:
            case WM_EXITMENULOOP:
            {
                if (m_FrameAction)
                {
                    break;
                }

                // HACK: disable the cursor once the user is done moving
                // or resizing the window or using the window menu
                if (m_CursorMode == DAIS_CURSOR_DISABLED)
                {
                    SetCursorEnabled(false);
                }

                break;
            }

            case WM_SIZE:
            {
                const int width = LOWORD(lParam);
                const int height = HIWORD(lParam);
                const bool minimized = wParam == SIZE_MINIMIZED;
                const bool maximized = wParam == SIZE_MAXIMIZED
                    || (m_Maximized && wParam != SIZE_RESTORED);

                if (WindowsPlatform::s_DisabledCursorWindow == this)
                {
                    UpdateClipRect(true);
                }

                if (m_Minimized != minimized)
                {
                    OnMinimize(minimized);
                }

                if (m_Maximized != maximized)
                {
                    OnMaximize(maximized);
                }

                if (width != m_Width
                    || height != m_Height)
                {
                    m_Width = width;
                    m_Height = height;

                    OnFramebufferSizeChanged(width, height);
                    OnSizeChanged(width, height);
                }

                if (m_Monitor
                    && m_Minimized != minimized)
                {
                    if (m_Minimized)
                    {
                        ReleaseMonitor();
                    }
                    else
                    {
                        AcquireMonitor();
                        FitToMonitor();
                    }
                }

                m_Minimized = minimized;
                m_Maximized = maximized;

                return 0;
            }

            case WM_MOVE:
            {
                if (WindowsPlatform::s_DisabledCursorWindow == this)
                {
                    UpdateClipRect(true);
                }

                // NOTE: this cannot use LOWORD/HIWORD recommended by MSDN, as
                // those macros do not handle negative window positions correctly
                OnPositionChanged(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

                return 0;
            }

            case WM_SIZING:
            {
                if (m_Numerator == -1
                    || m_Denominator == -1)
                {
                    break;
                }

                ApplyAspectRatio((int32_t)wParam, (RECT*)lParam);

                return TRUE;
            }

            case WM_GETMINMAXINFO:
            {
                if (m_Monitor)
                {
                    break;
                }

                UINT dpi = USER_DEFAULT_SCREEN_DPI;
                if (WindowsPlatform::IsWindows10AnniversaryUpdateOrGreater())
                {
                    dpi = WindowsPlatform::s_Libs.user32.GetDpiForWindow(m_Handle);
                }

                int xOffset, yOffset;
                GetFullSize(GetStyle(), GetStyleEx(), 0, 0, &xOffset, &yOffset, dpi);

                MINMAXINFO* mmi = (MINMAXINFO*)lParam;

                if (m_MinWidth != -1
                    && m_MinHeight != -1)
                {
                    mmi->ptMinTrackSize.x = m_MinWidth + xOffset;
                    mmi->ptMinTrackSize.y = m_MinHeight + yOffset;
                }

                if (m_MaxWidth != -1
                    && m_MaxHeight != -1)
                {
                    mmi->ptMaxTrackSize.x = m_MaxWidth + xOffset;
                    mmi->ptMaxTrackSize.y = m_MaxHeight + yOffset;
                }

                if (!m_Decorated)
                {
                    const HMONITOR mh = MonitorFromWindow(m_Handle, MONITOR_DEFAULTTONEAREST);

                    MONITORINFO mi = {};
                    mi.cbSize = sizeof(mi);
                    GetMonitorInfoW(mh, &mi);

                    mmi->ptMaxPosition.x = mi.rcWork.left - mi.rcMonitor.left;
                    mmi->ptMaxPosition.y = mi.rcWork.top - mi.rcMonitor.top;
                    mmi->ptMaxSize.x = mi.rcWork.right - mi.rcWork.left;
                    mmi->ptMaxSize.y = mi.rcWork.bottom - mi.rcWork.top;
                }

                return 0;
            }

            case WM_PAINT:
            {
                OnNeedUpdate();
                break;
            }

            case WM_ERASEBKGND:
            {
                return TRUE;
            }

            case WM_NCACTIVATE:
            case WM_NCPAINT:
            {
                // Prevent title bar from being drawn after restoring a minimized undecorated window
                if (!m_Decorated)
                    return TRUE;

                break;
            }

            case WM_DWMCOMPOSITIONCHANGED:
            case WM_DWMCOLORIZATIONCOLORCHANGED:
            {
                if (m_Transparent)
                    UpdateFramebufferTransparency();

                return 0;
            }

            case WM_GETDPISCALEDSIZE:
            {
                if (m_ScaleToMonitor)
                {
                    break;
                }

                // Adjust the window size to keep the content area size constant
                if (WindowsPlatform::IsWindows10CreatorsUpdateOrGreater())
                {
                    RECT source = { 0 }, target = { 0 };
                    SIZE* size = (SIZE*)lParam;

                    WindowsPlatform::s_Libs.user32.AdjustWindowRectExForDpi(&source,
                        GetStyle(), FALSE, GetStyleEx(),
                        WindowsPlatform::s_Libs.user32.GetDpiForWindow(m_Handle));

                    WindowsPlatform::s_Libs.user32.AdjustWindowRectExForDpi(&target,
                        GetStyle(), FALSE, GetStyleEx(),
                        LOWORD(wParam));

                    size->cx += (target.right - target.left) - (source.right - source.left);
                    size->cy += (target.bottom - target.top) - (source.bottom - source.top);

                    return TRUE;
                }

                break;
            }

            case WM_DPICHANGED:
            {
                const float xScale = HIWORD(wParam) / (float)USER_DEFAULT_SCREEN_DPI;
                const float yScale = LOWORD(wParam) / (float)USER_DEFAULT_SCREEN_DPI;

                // Resize windowed mode windows that either permit rescaling or that
                // need it to compensate for non-client area scaling
                if (!m_Monitor
                    && (m_ScaleToMonitor || WindowsPlatform::IsWindows10CreatorsUpdateOrGreater()))
                {
                    RECT* suggested = (RECT*)lParam;

                    SetWindowPos(m_Handle, HWND_TOP,
                        suggested->left,
                        suggested->top,
                        suggested->right - suggested->left,
                        suggested->bottom - suggested->top,
                        SWP_NOACTIVATE | SWP_NOZORDER);
                }

                OnContentScaleChanged(xScale, yScale);
                break;
            }

            case WM_SETCURSOR:
            {
                if (LOWORD(lParam) == HTCLIENT)
                {
                    UpdateCursorImage();
                    return TRUE;
                }

                break;
            }

            case WM_DROPFILES:
            {
                HDROP drop = (HDROP)wParam;
                const int count = DragQueryFileW(drop, 0xffffffff, NULL, 0);

                char** paths = (char**)calloc(count, sizeof(char*));

                // Move the mouse to the position of the drop
                POINT pt;
                DragQueryPoint(drop, &pt);

                OnCursorPositionChanged(pt.x, pt.y);

                for (int32_t i = 0; i < count; i++)
                {
                    const UINT length = DragQueryFileW(drop, i, NULL, 0);
                    WCHAR* buffer = (WCHAR*)calloc((size_t)length + 1, sizeof(WCHAR));

                    DragQueryFileW(drop, i, buffer, length + 1);
                    paths[i] = WindowsPlatform::WideStringToUTF8(buffer);

                    free(buffer);
                }

                OnDrop(count, (const char**)paths);

                for (int32_t i = 0; i < count; i++)
                {
                    free(paths[i]);
                }
                free(paths);

                DragFinish(drop);
                return 0;
            }
        }

        return DefWindowProcW(m_Handle, uMsg, wParam, lParam);
    }


    /// <summary> Make this window and its video mode active on its monitor </summary>
    void WindowsWindow::AcquireMonitor()
    {
        if (!WindowsPlatform::s_AcquiredMonitorCount)
        {
            SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);

            //HACK: when mouse trails are enabled, the cursor becomes invisible when
            // the OpenGL ICD switches to page flipping
            SystemParametersInfoW(SPI_GETMOUSETRAILS, 0, &WindowsPlatform::s_MouseTrailSize, 0);
            SystemParametersInfoW(SPI_SETMOUSETRAILS, 0, 0, 0);
        }

        if (!m_Monitor->GetWindow())
        {
            WindowsPlatform::s_AcquiredMonitorCount++;
        }

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

        WindowsPlatform::s_AcquiredMonitorCount--;
        if (WindowsPlatform::s_AcquiredMonitorCount)
        {
            SetThreadExecutionState(ES_CONTINUOUS);

            //HACK: restore mouse trail length saved in AcquireMonitor
            SystemParametersInfoW(SPI_SETMOUSETRAILS, WindowsPlatform::s_MouseTrailSize, 0, 0);
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

    void WindowsWindow::ChangeMessageFilter()
    {
        if (WindowsPlatform::IsWindows7OrGreater())
        {
            WindowsPlatform::s_Libs.user32.ChangeWindowMessageFilterEx(m_Handle, WM_DROPFILES, MSGFLT_ALLOW, NULL);
            WindowsPlatform::s_Libs.user32.ChangeWindowMessageFilterEx(m_Handle, WM_COPYDATA, MSGFLT_ALLOW, NULL);
            WindowsPlatform::s_Libs.user32.ChangeWindowMessageFilterEx(m_Handle, WM_COPYGLOBALDATA, MSGFLT_ALLOW, NULL);
        }
    }

    void WindowsWindow::UpdateFramebufferTransparency()
    {
        if (!WindowsPlatform::IsWindowsVistaOrGreater())
        {
            return;
        }

        BOOL composition;
        if (FAILED(WindowsPlatform::s_Libs.dwmapi.IsCompositionEnabled(&composition)) || !composition)
        {
            return;
        }

        BOOL opaque;
        DWORD color;
        if (WindowsPlatform::IsWindows8OrGreater()
            || (SUCCEEDED(WindowsPlatform::s_Libs.dwmapi.GetColorizationColor(&color, &opaque)) && !opaque))
        {
            HRGN region = CreateRectRgn(0, 0, -1, -1);
            DWM_BLURBEHIND bb = { 0 };
            bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
            bb.hRgnBlur = region;
            bb.fEnable = TRUE;

            WindowsPlatform::s_Libs.dwmapi.EnableBlurBehindWindow(m_Handle, &bb);
            DeleteObject(region);
        }
        else
        {
            // HACK: disable framebuffer transparency on Windows 7 when the
            // colorization color is opaque, because otherwise the window
            // contents is blended additively with the previous frame instead
            // of replacing it
            DWM_BLURBEHIND bb = { 0 };
            bb.dwFlags = DWM_BB_ENABLE;

            WindowsPlatform::s_Libs.dwmapi.EnableBlurBehindWindow(m_Handle, &bb);
        }
    }

    /// <summary> Enforce the content area aspect ratio based on which edge is being dragged </summary>
    void WindowsWindow::ApplyAspectRatio(int32_t edge, RECT* rect)
    {
        UINT dpi = USER_DEFAULT_SCREEN_DPI;
        const float ratio = (float)m_Numerator / (float)m_Denominator;

        if (WindowsPlatform::IsWindows10AnniversaryUpdateOrGreater())
        {
            dpi = WindowsPlatform::s_Libs.user32.GetDpiForWindow(m_Handle);
        }

        int32_t xOffset, yOffset;
        GetFullSize(GetStyle(), GetStyleEx(), 0, 0, &xOffset, &yOffset, dpi);

        if (edge == WMSZ_LEFT
            || edge == WMSZ_BOTTOMLEFT
            || edge == WMSZ_RIGHT
            || edge == WMSZ_BOTTOMRIGHT)
        {
            rect->bottom = rect->top + yOffset + (int32_t)((rect->right - rect->left - xOffset) / ratio);
        }
        else if (edge == WMSZ_TOPLEFT
            || edge == WMSZ_TOPRIGHT)
        {
            rect->top = rect->bottom - yOffset - (int32_t)((rect->right - rect->left - xOffset) / ratio);

        }
        else if (edge == WMSZ_TOP
            || edge == WMSZ_BOTTOM)
        {
            rect->right = rect->left + xOffset + (int32_t)((rect->bottom - rect->top - yOffset) * ratio);
        }
    }

    void WindowsWindow::SetCursorEnabled(bool enabled)
    {
        if (enabled)
        {
            if (m_RawMouseMotion)
            {
                PlatformSetRawMouseMotion(false);
            }

            WindowsPlatform::s_DisabledCursorWindow = nullptr;
            UpdateClipRect(false);
            PlatformSetCursorPosition(WindowsPlatform::s_RestoreCursorPositionX,
                WindowsPlatform::s_RestoreCursorPositionY);

            UpdateCursorImage();
        }
        else
        {
            WindowsPlatform::s_DisabledCursorWindow = this;
            PlatformGetCursorPosition(&WindowsPlatform::s_RestoreCursorPositionX,
                &WindowsPlatform::s_RestoreCursorPositionY);

            UpdateCursorImage();
            CenterCursorInContentArea();
            UpdateClipRect(true);

            if (m_RawMouseMotion)
            {
                PlatformSetRawMouseMotion(true);
            }
        }
    }

    void WindowsWindow::UpdateCursorImage()
    {
        if (m_CursorMode == DAIS_CURSOR_NORMAL)
        {
            if (m_Cursor)
            {
                SetCursor(((WindowsCursor*)m_Cursor)->m_Handle);
            }
            else
            {
                SetCursor(LoadCursorW(NULL, IDC_ARROW));
            }
        }
        else
        {
            SetCursor(NULL);
        }
    }

    void WindowsWindow::UpdateClipRect(bool clipToWindow)
    {
        if (clipToWindow)
        {
            RECT clipRect;
            GetClientRect(m_Handle, &clipRect);
            ClientToScreen(m_Handle, (POINT*)&clipRect.left);
            ClientToScreen(m_Handle, (POINT*)&clipRect.right);
            ClipCursor(&clipRect);
        }
        else
        {
            ClipCursor(NULL);
        }
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

        WindowsPlatform::AdjustRect(&rect, m_Handle, style, styleEx);

        ClientToScreen(m_Handle, (POINT*)&rect.left);
        ClientToScreen(m_Handle, (POINT*)&rect.right);

        SetWindowLongW(m_Handle, GWL_STYLE, style);
        SetWindowPos(m_Handle, HWND_TOP,
            rect.left, rect.top,
            rect.right - rect.left, rect.bottom - rect.top,
            SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOZORDER);
    }

    void WindowsWindow::AdjustRect(RECT* rect) const
    {
        WindowsPlatform::AdjustRect(rect, m_Handle, GetStyle(), GetStyleEx());
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

    int32_t WindowsWindow::GetKeyMods() const
    {
        int32_t mods = 0;

        if (GetKeyState(VK_SHIFT) & 0x8000)
        {
            mods |= DAIS_MOD_SHIFT;
        }

        if (GetKeyState(VK_CONTROL) & 0x8000)
        {
            mods |= DAIS_MOD_CONTROL;
        }

        if (GetKeyState(VK_MENU) & 0x8000)
        {
            mods |= DAIS_MOD_ALT;
        }

        if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
        {
            mods |= DAIS_MOD_SUPER;
        }

        if (GetKeyState(VK_CAPITAL) & 1)
        {
            mods |= DAIS_MOD_CAPS_LOCK;
        }

        if (GetKeyState(VK_NUMLOCK) & 1)
        {
            mods |= DAIS_MOD_NUM_LOCK;
        }

        return mods;
    }
}

