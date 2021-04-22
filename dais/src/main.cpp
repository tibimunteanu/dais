#include "engine/core/Platform.h"

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
    dais::WindowConfig windowConfig = {};
    windowConfig.title = "Test";
    windowConfig.width = 960;
    windowConfig.height = 540;
    windowConfig.resizable = true;
    windowConfig.visible = true;
    windowConfig.decorated = true;
    windowConfig.focused = true;
    windowConfig.autoIconify = true;
    windowConfig.centerCursor = true;
    windowConfig.focusOnShow = true;

    windowConfig.scaleToMonitor = true;
    windowConfig.refreshRate = -1;

    dais::FramebufferConfig fbConfig = {};
    fbConfig.redBits = 8;
    fbConfig.greenBits = 8;
    fbConfig.blueBits = 8;
    fbConfig.alphaBits = 8;
    fbConfig.depthBits = 24;
    fbConfig.stencilBits = 8;
    fbConfig.doubleBuffer = true;
    fbConfig.sRGB = true;
    
    dais::Window* window = dais::Platform::OpenWindow(windowConfig, fbConfig, nullptr);
    if (!window)
    {
        throw std::runtime_error("Could not open window!");
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
