#pragma once

#include "platform/windows/WindowsBase.h"

namespace dais
{
    class WindowsWindow : public Window
    {
    private:
        HWND m_Handle = nullptr;
        bool m_Minimized = false;
        bool m_Maximized = false;
        bool m_ScaleToMonitor = false;

    public:
        static void GetFullSize(DWORD style, DWORD styleEx,
            int contentWidth, int contentHeight,
            int* fullWidth, int* fullHeight,
            UINT dpi);

        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    public:
        WindowsWindow(WindowConfig config, FramebufferConfig fbConfig, Monitor* monitor);

        virtual ~WindowsWindow();

    public:
        bool PlatformIsMaximized() const override;
        bool PlatformIsMinimized() const override;
        bool PlatformIsVisible() const override;
        bool PlatformIsHovered() const override;
        bool PlatformIsFocused() const override;

        void PlatformGetPosition(int32_t* x, int32_t* y) const override;
        void PlatformGetSize(int32_t* width, int32_t* height) const override;
        void PlatformGetFrameSize(int32_t* left, int32_t* top, int32_t* right, int32_t* bottom) const override;
        void PlatformGetContentScale(float* xScale, float* yScale) override;
        void* PlatformGetHandle() const override;

        void PlatformSetTitle(const std::string& title) override;
        void PlatformSetPosition(int32_t x, int32_t y) override;
        void PlatformSetSize(int32_t width, int32_t height) override;
        void PlatformSetDecorated(bool value) override;
        void PlatformSetFloating(bool value) override;
        void PlatformSetResizable(bool value) override;
        void PlatformSetMousePassThrough(bool value) override;
        void PlatformSetMonitor(Monitor* monitor, int32_t x, int32_t y, int32_t width, int32_t height, int32_t refreshRate) override;

        void PlatformMaximize() override;
        void PlatformMinimize() override;
        void PlatformRestore() override;
        void PlatformShow() override;
        void PlatformHide() override;
        void PlatformRequestAttention() override;
        void PlatformFocus() override;

    public:
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

        void AcquireMonitor();
        void ReleaseMonitor();
        void FitToMonitor();

        DWORD GetStyle() const;
        DWORD GetStyleEx() const;
        void UpdateStyles();
        void AdjustRect(RECT* rect) const;
        bool IsCursorInContentArea() const;
    };
}
