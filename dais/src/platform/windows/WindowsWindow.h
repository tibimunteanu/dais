#pragma once

#include "platform/windows/WindowsBase.h"

namespace dais
{
    class WindowsWindow : public Window
    {
    private:
        HWND m_Handle = nullptr;
        HICON m_SmallIcon = nullptr;
        HICON m_BigIcon = nullptr;
        bool m_Minimized = false;
        bool m_Maximized = false;
        bool m_ScaleToMonitor = false;
        bool m_Transparent = false; //whether to enable framebuffer transparency on DWM
        bool m_FrameAction = false;
        bool m_CursorTracked = false;
        bool m_KeyMenu = false;
        int32_t m_LastCursorPositionX = 0;
        int32_t m_LastCursorPositionY = 0;
        WCHAR m_HighSurrogate = {}; //the last received high surrogate when decoding pairs of UTF-16 messages

    public: DAIS_INTERNAL_API
        static void GetFullSize(DWORD style, DWORD styleEx, int contentWidth, int contentHeight, int* fullWidth, int* fullHeight, UINT dpi);
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static HICON CreateIcon(const Image* image, int32_t xHot, int32_t yHot, bool icon);

    public:
        WindowsWindow(const std::string& title, int32_t width, int32_t height,
                      const WindowConfig* windowConfig, const ContextConfig* contextConfig, const FramebufferConfig* framebufferConfig, Monitor* monitor);

        virtual ~WindowsWindow();
        friend class Window;
        friend class WglContext;

    private: DAIS_PLATFORM_API
        bool PlatformIsMaximized() const override;
        bool PlatformIsMinimized() const override;
        bool PlatformIsVisible() const override;
        bool PlatformIsHovered() const override;
        bool PlatformIsFocused() const override;
        bool PlatformIsFramebufferTransparent() const override;

        void PlatformGetPosition(int32_t* x, int32_t* y) const override;
        void PlatformGetSize(int32_t* width, int32_t* height) const override;
        void PlatformGetFramebufferSize(int32_t* width, int32_t* height) const override;
        void PlatformGetFrameSize(int32_t* left, int32_t* top, int32_t* right, int32_t* bottom) const override;
        void PlatformGetContentScale(float* xScale, float* yScale) override;
        void PlatformGetCursorPosition(double* x, double* y) override;
        float PlatformGetOpacity() override;
        const char* PlatformGetClipboardString() override;
        void* PlatformGetHandle() const override;

        void PlatformSetTitle(const std::string& title) override;
        void PlatformSetIcon(const std::vector<Image*>& images) override;
        void PlatformSetCursorType(Cursor* cursor) override;
        void PlatformSetPosition(int32_t x, int32_t y) override;
        void PlatformSetSize(int32_t width, int32_t height) override;
        void PlatformSetSizeLimits(int32_t minWidth, int32_t minHeight, int32_t maxWidth, int32_t maxHeight) override;
        void PlatformSetAspectRatio(int32_t numerator, int32_t denominator) override;
        void PlatformSetOpacity(float opacity) override;
        void PlatformSetDecorated(bool value) override;
        void PlatformSetFloating(bool value) override;
        void PlatformSetResizable(bool value) override;
        void PlatformSetMousePassThrough(bool value) override;
        void PlatformSetClipboardString(const char* string) override;
        void PlatformSetMonitor(Monitor* monitor, int32_t x, int32_t y, int32_t width, int32_t height, int32_t refreshRate) override;
        void PlatformSetCursor(Cursor* cursor) override;
        void PlatformSetCursorPosition(double x, double y) override;
        void PlatformSetCursorMode(CursorMode mode) override;
        void PlatformSetRawMouseMotion(bool enabled) override;

        void PlatformMaximize() override;
        void PlatformMinimize() override;
        void PlatformRestore() override;
        void PlatformShow() override;
        void PlatformHide() override;
        void PlatformRequestAttention() override;
        void PlatformFocus() override;

    private: DAIS_UTILS
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

        void AcquireMonitor();
        void ReleaseMonitor();
        void FitToMonitor();
        void ChangeMessageFilter();
        void UpdateFramebufferTransparency();
        void ApplyAspectRatio(int32_t edge, RECT* rect);
        void SetCursorEnabled(bool enabled);
        void UpdateCursorImage();
        void UpdateClipRect(bool clipToWindow);
        DWORD GetStyle() const;
        DWORD GetStyleEx() const;
        void UpdateStyles();
        void AdjustRect(RECT* rect) const;
        bool IsCursorInContentArea() const;
        KeyMods GetKeyMods() const;
    };
}
