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
        HWND m_HelperWindowHandle = nullptr;
        HDEVNOTIFY m_DeviceNotificationHandle = nullptr;
        DWORD m_ForegroundLockTimeout = 0;

    public:
        WindowsPlatform();
        virtual ~WindowsPlatform();

        void Init() override;

        virtual void PollEvents() override;
        virtual void WaitEvents() override;
        virtual void WaitEventsTimeout(double timeout) override;

    private:
        void PollMonitors();
        void SetForegroundLockTimeout();
        void RestoreForegroundLockTimeout();
        bool RegisterWindowClass();
        void UnregisterWindowClass();
        bool CreateHelperWindow();
    };
}

