#include "engine/core/Platform.h"

#pragma comment(lib, "opengl32")
#include <GL/gl.h>

static void OnMonitorConnected(dais::Monitor* monitor)
{
    std::cout << "[MAIN] OnMonitorConnected: " << std::endl << "\t" << monitor->GetName() << std::endl;
}

static void OnMonitorDisconnected(dais::Monitor* monitor)
{
    std::cout << "[MAIN] OnMonitorDisconnected: " << std::endl << "\t" << monitor->GetName() << std::endl;
}

int main(int argc, char** argv)
{
    dais::Platform::SetMonitorConnectedCallback(OnMonitorConnected);
    dais::Platform::SetMonitorDisconnectedCallback(OnMonitorDisconnected);

    dais::Platform::Init();

    //test window api
    dais::Platform::s_Hints = {};
    dais::Platform::s_Hints.refreshRate = -1;
    dais::Platform::s_Hints.window.title = "Test";
    dais::Platform::s_Hints.window.width = 960;
    dais::Platform::s_Hints.window.height = 540;
    dais::Platform::s_Hints.window.resizable = true;
    dais::Platform::s_Hints.window.visible = true;
    dais::Platform::s_Hints.window.decorated = true;
    dais::Platform::s_Hints.window.focused = true;
    dais::Platform::s_Hints.window.autoIconify = true;
    dais::Platform::s_Hints.window.centerCursor = true;
    dais::Platform::s_Hints.window.focusOnShow = true;
    dais::Platform::s_Hints.window.scaleToMonitor = true;
    dais::Platform::s_Hints.context.client = DAIS_OPENGL_API;
    dais::Platform::s_Hints.context.source = DAIS_NATIVE_CONTEXT_API;
    dais::Platform::s_Hints.context.major = 1;
    dais::Platform::s_Hints.context.minor = 0;
    dais::Platform::s_Hints.framebuffer.redBits = 8;
    dais::Platform::s_Hints.framebuffer.greenBits = 8;
    dais::Platform::s_Hints.framebuffer.blueBits = 8;
    dais::Platform::s_Hints.framebuffer.alphaBits = 8;
    dais::Platform::s_Hints.framebuffer.depthBits = 24;
    dais::Platform::s_Hints.framebuffer.stencilBits = 8;
    dais::Platform::s_Hints.framebuffer.doubleBuffer = true;
    dais::Platform::s_Hints.framebuffer.sRGB = true;

    dais::Window* window = dais::Platform::OpenWindow(nullptr);
    if (!window)
    {
        throw std::runtime_error("Could not open window!");
    }

    std::cout 
        << "OpenGL "
        << window->GetContext()->m_Major << "." << window->GetContext()->m_Minor << "." << window->GetContext()->m_Revision
        << std::endl;

    //const char* glVersion = (const char*)window->GetContext()->GetString(GL_VERSION);
    const char* glVendor = (const char*)window->GetContext()->GetString(GL_VENDOR);
    //const char* glRenderer = (const char*)glGetString(GL_RENDERER);
    if (!glVendor)
    {
        GLenum err = glGetError();
        switch (err)
        {
            case GL_INVALID_ENUM: std::cout << "GL Invalid enum" << std::endl; break;
            case GL_INVALID_VALUE: std::cout << "GL Invalid value" << std::endl; break;
            case GL_INVALID_OPERATION: std::cout << "GL Invalid operation" << std::endl; break;
            case GL_OUT_OF_MEMORY: std::cout << "GL Out of memory" << std::endl; break;
            case GL_NO_ERROR: std::cout << "GL No error" << std::endl; break;
        }
    }
    else
    {
        std::cout << glVendor << std::endl;
    }

    while (!window->ShouldClose())
    {
        dais::Platform::PollEvents();
    }

    //test monitor api
    const std::vector<dais::Monitor*>& monitors = dais::Platform::GetMonitors();
    for (int32_t m = 0; m < monitors.size(); m++)
    {
        std::cout << "\t" << monitors[m]->GetName() << std::endl;
    }

    dais::Monitor* currentMonitor = dais::Platform::GetPrimaryMonitor();
    if (currentMonitor)
    {
        std::cout << "\t" << currentMonitor->GetName() << std::endl;

        const std::vector<dais::VideoMode*>& videoModes = currentMonitor->GetVideoModes();

        std::cout << "\t" << videoModes.size() << " modes" << std::endl;
        for (int32_t m = 0; m < videoModes.size(); m++)
        {
            std::cout << "\t" << *videoModes[m] << std::endl;
        }

        dais::VideoMode* currentVideoMode = currentMonitor->GetVideoMode();
        if (currentVideoMode)
        {
            std::cout << "\t" << *currentVideoMode << std::endl;
        }

        int32_t x, y;
        currentMonitor->GetPosition(&x, &y);
        std::cout << "\tx: " << x << ", y: " << y << std::endl;

        int32_t w, h;
        currentMonitor->GetWorkarea(&x, &y, &w, &h);
        std::cout << "\tx: " << x << ", y: " << y << ", w: " << w << ", h: " << h << std::endl;

        currentMonitor->GetPhysicalSize(&w, &h);
        std::cout << "\tw: " << w << "mm, h: " << h << "mm" << std::endl;

        float xScale, yScale;
        currentMonitor->GetContentScale(&xScale, &yScale);
        std::cout << "\txScale: " << xScale << ", yScale: " << yScale << std::endl;
    }

    dais::Platform::Terminate();

    return 0;
}
