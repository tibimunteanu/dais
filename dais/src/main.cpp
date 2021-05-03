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
static void OnWindowPositionChanged(dais::Window* window, int32_t x, int32_t y)
{
    std::cout << "[MAIN] OnWindowPosition: " << x << ", " << y << std::endl;
}
static void OnWindowSizeChanged(dais::Window* window, int32_t width, int32_t height)
{
    std::cout << "[MAIN] OnWindowSize: " << width << ", " << height << std::endl;
}
static void OnWindowClose(dais::Window* window)
{
    std::cout << "[MAIN] OnWindowClose" << std::endl;
}
static void OnWindowRefreshNeeded(dais::Window* window)
{
    std::cout << "[MAIN] OnWindowRefresh" << std::endl;
}
static void OnWindowFocus(dais::Window* window, bool focused)
{
    std::cout << "[MAIN] OnWindowFocus: " << focused << std::endl;
}
static void OnWindowMinimize(dais::Window* window, bool minimized)
{
    std::cout << "[MAIN] OnWindowMinimize: " << minimized << std::endl;
}
static void OnWindowMaximize(dais::Window* window, bool maximized)
{
    std::cout << "[MAIN] OnWindowMaximize: " << maximized << std::endl;
}
static void OnWindowFramebufferSizeChanged(dais::Window* window, int32_t width, int32_t height)
{
    std::cout << "[MAIN] OnWindowFramebufferSize: " << width << ", " << height << std::endl;
}
static void OnWindowContentScaleChanged(dais::Window* window, float xScale, float yScale)
{
    std::cout << "[MAIN] OnWindowContentScale: " << xScale << ", " << yScale << std::endl;
}
static void OnWindowMouseButton(dais::Window* window, dais::MouseButton button, dais::KeyState state, dais::KeyMods mods)
{
    std::cout 
        << "[MAIN] OnWindowMouseButton: " 
        << (int32_t)button 
        << ", " 
        << (state == dais::KeyState::Press ? "Press" : state == dais::KeyState::Release ? "Release" : "Repeat") 
        << ((mods & dais::KeyMods::Alt) == dais::KeyMods::Alt ? ":Alt:" : "")
        << ((mods & dais::KeyMods::Shift) == dais::KeyMods::Shift ? ":Shift:" : "")
        << ((mods & dais::KeyMods::Control) == dais::KeyMods::Control ? ":Ctrl:" : "")
        << ((mods & dais::KeyMods::Super) == dais::KeyMods::Super ? ":Super:" : "")
        << ((mods & dais::KeyMods::CapsLock) == dais::KeyMods::CapsLock ? ":CapsLock:" : "")
        << ((mods & dais::KeyMods::NumLock) == dais::KeyMods::NumLock ? ":NumLock:" : "")
        << std::endl;
}
static void OnCursorPositionChanged(dais::Window* window, double x, double y)
{
    std::cout << "[MAIN] OnCursorPositionChanged: " << x << ", " << y << std::endl;
}
static void OnWindowCursorEnter(dais::Window* window, bool entered)
{
    std::cout << "[MAIN] OnWindowCursorEnter: " << entered << std::endl;
}
static void OnWindowScroll(dais::Window* window, double xOffset, double yOffset)
{
    std::cout << "[MAIN] OnWindowScroll: " << xOffset << ", " << yOffset << std::endl;
}
static void OnWindowKey(dais::Window* window, dais::Key key, int32_t scancode, dais::KeyState state, dais::KeyMods mods)
{
    std::cout 
        << "[MAIN] OnWindowKey: " 
        << (int32_t)key 
        << ", " 
        << (state == dais::KeyState::Press ? "Press" : state == dais::KeyState::Release ? "Release" : "Repeat") 
        << ((mods & dais::KeyMods::Alt) == dais::KeyMods::Alt ? ":Alt:" : "")
        << ((mods & dais::KeyMods::Shift) == dais::KeyMods::Shift ? ":Shift:" : "")
        << ((mods & dais::KeyMods::Control) == dais::KeyMods::Control ? ":Ctrl:" : "")
        << ((mods & dais::KeyMods::Super) == dais::KeyMods::Super ? ":Super:" : "")
        << ((mods & dais::KeyMods::CapsLock) == dais::KeyMods::CapsLock ? ":CapsLock:" : "")
        << ((mods & dais::KeyMods::NumLock) == dais::KeyMods::NumLock ? ":NumLock:" : "")
        << std::endl;
}
static void OnWindowChar(dais::Window* window, uint32_t codepoint)
{
    std::cout << "[MAIN] OnWindowChar: " << (char)codepoint << std::endl;
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

    bool hasContext = dais::Platform::s_Hints.context.api != dais::ContextAPI::None;
    if (hasContext)
    {
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
    }

    window->SetCharCallback(OnWindowChar);
    window->SetKeyCallback(OnWindowKey);
    //window->SetCursorPositionCallback(OnCursorPositionChanged);
    window->SetCloseCallback(OnWindowClose);
    window->SetContentScaleCallback(OnWindowContentScaleChanged);
    window->SetCursorEnterCallback(OnWindowCursorEnter);
    window->SetFocusCallback(OnWindowFocus);
    window->SetFramebufferSizeCallback(OnWindowFramebufferSizeChanged);
    window->SetMaximizeCallback(OnWindowMaximize);
    window->SetMinimizeCallback(OnWindowMinimize);
    window->SetMouseButtonCallback(OnWindowMouseButton);
    window->SetPositionCallback(OnWindowPositionChanged);
    window->SetScrollCallback(OnWindowScroll);
    window->SetSizeCallback(OnWindowSizeChanged);

    //loop
    double prevTime = dais::Platform::GetTime() - (1.0 / 60.0);
    double dt;
    while (!window->ShouldClose())
    {
        double time = dais::Platform::GetTime();
        dt = time - prevTime;
        prevTime = time;

        if (hasContext)
        {
            glClearColor(0.3f, 0.5f, 0.8f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            dais::Context::SwapBuffers(window);
        }

        dais::Platform::PollEvents();
    }

    dais::Platform::Terminate();

    return 0;
}
