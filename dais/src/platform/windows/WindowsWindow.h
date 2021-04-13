#pragma once

#include "platform/windows/WindowsBase.h"
#include "platform/windows/WindowsMonitor.h"
#include "engine/core/Window.h"

namespace dais
{
    class WindowsWindow : public Window
    {
    private:
        HWND m_Handle = nullptr;

    public:
        WindowsWindow(const std::string& title, uint32_t width, uint32_t height);
        virtual ~WindowsWindow();

        virtual void* GetHandle() const override; // Get native window handle

        virtual void* GetUserData() const override;
        virtual void SetUserData(void* data) override;

        virtual uint32_t GetMonitorCount() const override;
        virtual uint32_t GetMonitor() const override;
        virtual void SetMonitor(uint32_t monitor) override;

        virtual const std::string& GetTitle() const override;
        virtual void SetTitle(const std::string& title) override;

        virtual bool IsFullscreen() const override;
        virtual void SetFullscreen(bool fullscreen) override; 

        virtual void GetSize(uint32_t* width, uint32_t& height) const override;
        virtual void SetSize(uint32_t width, uint32_t height) override;

        virtual void GetPosition(uint32_t* x, uint32_t* y) const override;
        virtual void SetPosition(uint32_t x, uint32_t y) override;

        virtual bool IsMaximized() const override;
        virtual void Maximize() override;

        virtual bool IsMinimized() const override;
        virtual void Minimize() override;

        virtual void Restore() override;
        virtual bool ShouldClose() const override; // Close icon pressed
        virtual void Close() override;
    };
}
