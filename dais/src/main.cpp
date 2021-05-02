#include "engine/core/Platform.h"

#include "external/glad.h"

static void OnMonitorConnected(dais::Monitor* monitor)
{
    std::cout << "[MAIN] OnMonitorConnected: " << monitor->GetName() << std::endl;
}

static void OnMonitorDisconnected(dais::Monitor* monitor)
{
    std::cout << "[MAIN] OnMonitorDisconnected: " << monitor->GetName() << std::endl;
}

int main(int argc, char** argv)
{
    dais::Platform::SetMonitorConnectedCallback(OnMonitorConnected);
    dais::Platform::SetMonitorDisconnectedCallback(OnMonitorDisconnected);

    dais::Platform::s_Hints = {};
    dais::Platform::s_Hints.init.angleType = dais::AnglePlatformType::None;

    dais::Platform::Init();

    //test monitor api
    std::cout << "Connected monitors: " << std::endl;
    const std::vector<dais::Monitor*>& monitors = dais::Platform::GetMonitors();
    for (int32_t m = 0; m < monitors.size(); m++)
    {
        std::cout << "\t" << monitors[m]->GetName() << std::endl;
    }

    dais::Monitor* currentMonitor = dais::Platform::GetPrimaryMonitor();
    if (currentMonitor)
    {
        std::cout << "Current monitor: " << currentMonitor->GetName() << std::endl;

        const std::vector<dais::VideoMode*>& videoModes = currentMonitor->GetVideoModes();

        std::cout << "Video modes (" << videoModes.size() << "):" << std::endl;
        for (int32_t m = 0; m < videoModes.size(); m++)
        {
            std::cout << "\t" << *videoModes[m] << std::endl;
        }

        dais::VideoMode* currentVideoMode = currentMonitor->GetVideoMode();
        if (currentVideoMode)
        {
            std::cout << "Current video mode: " << *currentVideoMode << std::endl;
        }

        int32_t x, y;
        currentMonitor->GetPosition(&x, &y);
        std::cout << "Position: x: " << x << ", y: " << y << std::endl;

        int32_t w, h;
        currentMonitor->GetWorkarea(&x, &y, &w, &h);
        std::cout << "Workarea: x: " << x << ", y: " << y << ", w: " << w << ", h: " << h << std::endl;

        currentMonitor->GetPhysicalSize(&w, &h);
        std::cout << "Physical size: w: " << w << "mm, h: " << h << "mm" << std::endl;

        float xScale, yScale;
        currentMonitor->GetContentScale(&xScale, &yScale);
        std::cout << "Content scale: xScale: " << xScale << ", yScale: " << yScale << std::endl;
    }

    //test window api
    dais::Platform::s_Hints.refreshRate = -1;

    dais::Platform::s_Hints.window.resizable = true;
    dais::Platform::s_Hints.window.visible = true;
    dais::Platform::s_Hints.window.decorated = true;
    dais::Platform::s_Hints.window.focused = true;
    dais::Platform::s_Hints.window.autoMinimize = true;
    dais::Platform::s_Hints.window.centerCursor = true;
    dais::Platform::s_Hints.window.focusOnShow = true;
    dais::Platform::s_Hints.window.scaleToMonitor = true;

    dais::Platform::s_Hints.context.api = dais::ContextAPI::OpenGL;
    dais::Platform::s_Hints.context.type = dais::ContextType::Native;
    dais::Platform::s_Hints.context.profile = dais::ContextProfile::Core;
    dais::Platform::s_Hints.context.robustness = dais::ContextRobustnessMode::None;
    dais::Platform::s_Hints.context.release = dais::ContextReleaseBehavior::Any;
    dais::Platform::s_Hints.context.debug = false;
    dais::Platform::s_Hints.context.noerror = false;
    dais::Platform::s_Hints.context.forward = false;
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

    dais::Window* window = dais::Platform::OpenWindow("Dais", 960, 540, nullptr);
    if (!window)
    {
        std::cout << "Could not open window!" << std::endl;
        return 1;
    }

    std::cout 
        << "Current context: OpenGL "
        << window->GetContext()->m_Major << "." << window->GetContext()->m_Minor << "." << window->GetContext()->m_Revision
        << std::endl;

    dais::Context::MakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)dais::Context::GetGLProcAddress))
    {
        std::cout << "Could not initialize OpenGL loader!" << std::endl;
        return 1;
    }

    dais::Context::SwapInterval(1);

    //loop
    while (!window->ShouldClose())
    {
        glClearColor(0.3f, 0.5f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        dais::Context::SwapBuffers(window);
        dais::Platform::PollEvents();
    }

    dais::Platform::Terminate();

    return 0;
}
