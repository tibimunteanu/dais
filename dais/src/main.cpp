#include "engine/core/Platform.h"

static void OnMonitorConnected(dais::Monitor* monitor)
{
    std::cout << "Monitor connected callback: " << monitor->GetName() << std::endl;
}

static void OnMonitorDisconnected(dais::Monitor* monitor)
{
    std::cout << "Monitor disconnected callback: " << monitor->GetName() << std::endl;
}

int main(int argc, char** argv)
{
    dais::Platform* platform = dais::Platform::Create(
    {
        OnMonitorConnected,
        OnMonitorDisconnected
    });

    dais::Window* window = platform->OpenWindow("Test", 960, 540);
    if (!window)
    {
        throw std::runtime_error("Could not open window!");
    }

    const std::vector<dais::Monitor*> monitors = platform->GetMonitors();
    std::cout << "GetMonitors result:" << std::endl;
    for (int32_t m = 0; m < monitors.size(); m++)
    {
        std::cout << "\t" << monitors[m]->GetName() << std::endl;
    }

    dais::Monitor* currentMonitor = platform->GetPrimaryMonitor();
    if (currentMonitor)
    {
        std::cout << "GetPrimaryMonitor result" << std::endl << "\t" << currentMonitor->GetName() << std::endl;

        int32_t videoModeCount;
        dais::VideoMode* videoModes = currentMonitor->GetVideoModes(&videoModeCount);
    }

    window->Close();

    delete platform;

    return 0;
}
