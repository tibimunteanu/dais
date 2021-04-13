#include "engine/core/Window.h"

namespace dais
{
    Window::Window(const std::string& title, uint32_t width, uint32_t height)
    {
        std::cout << "[Window] Constructor" << std::endl;

        m_Title = title;
        m_Width = width;
        m_Height = height;
    }

    Window::~Window()
    {
        std::cout << "[Window] Destructor" << std::endl;
    }
}
