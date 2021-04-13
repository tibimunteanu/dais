#include "platform/windows/WindowsWindow.h"

namespace dais
{
    Window* Window::Create(const std::string& title, uint32_t width, uint32_t height)
    {
        return new WindowsWindow(title, width, height);
    }

    WindowsWindow::WindowsWindow(const std::string& title, uint32_t width, uint32_t height)
        : Window(title, width, height)
    {
        std::cout << "[WindowsWindow] Constructor" << std::endl;
    }

    WindowsWindow::~WindowsWindow()
    {
        std::cout << "[WindowsWindow] Destructor" << std::endl;
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

    void WindowsWindow::GetSize(uint32_t* width, uint32_t& height) const
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
        std::cout << "[WindowsWindow] Close" << std::endl;
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

                SetPropW(hwnd, L"DAIS_WINDOW", pThis);
            }
        }
        else
        {
            pThis = (WindowsWindow*)GetPropW(hwnd, L"DAIS_WINDOW");
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
                            //DetectJoystickConnection();
                        }
                    }
                    else if (wParam == DBT_DEVICEREMOVECOMPLETE)
                    {
                        DEV_BROADCAST_HDR* dbh = (DEV_BROADCAST_HDR*)lParam;
                        if (dbh && dbh->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
                        {
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

