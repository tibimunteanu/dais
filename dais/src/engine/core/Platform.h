#pragma once

#include "engine/core/Base.h"
#include "engine/core/Monitor.h"
#include "engine/core/Window.h"

typedef void(*MonitorConnectedCallback)(dais::Monitor*);

namespace dais
{
    class Platform
    {
    protected:
        std::vector<Window*> m_Windows = {};
        std::vector<Monitor*> m_Monitors = {};

    protected:
        struct Callbacks
        {
            MonitorConnectedCallback MonitorConnected;
            MonitorConnectedCallback MonitorDisconnected;
        } m_Callbacks = {};

    protected:
        Platform();

    public:
        virtual ~Platform();

        virtual void Init() = 0;

        Window* OpenWindow(const std::string& title, uint32_t width, uint32_t height);
        const std::vector<Window*>& GetWindows() const;
        Window* GetPrimaryWindow() const;

        const std::vector<Monitor*>& GetMonitors() const;
        Monitor* GetPrimaryMonitor() const;

        void SetMonitorConnectedCallback(MonitorConnectedCallback callback);
        void SetMonitorDisconnectedCallback(MonitorConnectedCallback callback);

    public:
        static Platform* Create();
    };
}
