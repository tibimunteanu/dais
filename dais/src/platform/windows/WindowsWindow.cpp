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

        m_Handle = nullptr;
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
}
