#include "engine/core/Window.h"

namespace dais
{
    Window::Window(WindowConfig config, Monitor* monitor)
    {
        DAIS_TRACE("[Window] Constructor");

        m_Title = config.title;
        m_Width = config.width;
        m_Height = config.height;
        m_Decorated = config.decorated;
        m_Visible = config.visible;
        m_Focused = config.focused;
        m_FocusOnShow = config.focusOnShow;
        m_AutoIconify = config.autoIconify;
        m_Floating = config.floating;
        m_Maximized = config.maximized;
        m_Resizable = config.resizable;
        m_CenterCursor = config.centerCursor;
        m_MousePassthrough = config.mousePassthrough;
        m_ScaleToMonitor = config.scaleToMonitor;
        m_Monitor = monitor;
    }

    Window::~Window()
    {
        DAIS_TRACE("[Window] Destructor");
    }
}
