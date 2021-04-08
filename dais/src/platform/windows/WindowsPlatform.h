#pragma once

#include "platform/windows/WindowsBase.h"
#include "platform/windows/WindowsMonitor.h"
#include "platform/windows/WindowsWindow.h"
#include "engine/core/Platform.h"

namespace dais
{
    class WindowsPlatform : public Platform
    {
    public:
        WindowsPlatform(Callbacks callbacks);
        virtual ~WindowsPlatform();

    private:
        void PollMonitors();
    };
}

