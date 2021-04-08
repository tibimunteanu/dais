#pragma once

#include "engine/core/Base.h"
#include "engine/core/Monitor.h"

namespace dais
{
    class Window
    {
    protected:
        std::string m_Title;
        uint32_t m_Width;
        uint32_t m_Height;
        Monitor* m_Monitor;

    protected:
        Window(const std::string& title, uint32_t width, uint32_t height);

    public:
        virtual ~Window();

        virtual void* GetHandle() const = 0; // Get native window handle

        virtual void* GetUserData() const = 0;
        virtual void SetUserData(void* data) = 0;

        virtual uint32_t GetMonitorCount() const = 0;
        virtual uint32_t GetMonitor() const = 0;
        virtual void SetMonitor(uint32_t monitor) = 0;

        virtual const std::string& GetTitle() const = 0;
        virtual void SetTitle(const std::string& title) = 0;

        virtual void GetSize(uint32_t* width, uint32_t& height) const = 0;
        virtual void SetSize(uint32_t width, uint32_t height) = 0;

        virtual void GetPosition(uint32_t* x, uint32_t* y) const = 0;
        virtual void SetPosition(uint32_t x, uint32_t y) = 0;

        virtual bool IsFullscreen() const = 0;
        virtual void SetFullscreen(bool fullscreen) = 0; 

        virtual bool IsMaximized() const = 0;
        virtual void Maximize() = 0;

        virtual bool IsMinimized() const = 0;
        virtual void Minimize() = 0;

        virtual void Restore() = 0;
        virtual bool ShouldClose() const = 0; // Close icon pressed
        virtual void Close() = 0;

    public:
        static Window* Create(const std::string& title, uint32_t width, uint32_t height);
    };
}
