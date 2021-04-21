#pragma once

#include "engine/core/Base.h"

class dais::Monitor;

namespace dais
{
    typedef void(*WindowPositionCallback)(Window*, int32_t, int32_t);
    typedef void(*WindowSizeCallback)(Window*, int32_t, int32_t);
    typedef void(*WindowCloseCallback)(Window*);
    typedef void(*WindowRefreshCallback)(Window*);
    typedef void(*WindowFocusCallback)(Window*, bool);
    typedef void(*WindowMinimizeCallback)(Window*, bool);
    typedef void(*WindowMaximizeCallback)(Window*, bool);
    typedef void(*WindowFramebufferSizeCallback)(Window*, int32_t, int32_t);
    typedef void(*WindowContentScaleCallback)(Window*, float, float);
    typedef void(*WindowMouseButtonCallback)(Window*, int32_t, int32_t, int32_t);
    typedef void(*WindowCursorPositionCallback)(Window*, double, double);
    typedef void(*WindowCursorEnterCallback)(Window*, bool);
    typedef void(*WindowScrollCallback)(Window*, double, double);
    typedef void(*WindowKeyCallback)(Window*, int32_t, int32_t, int32_t, int32_t);
    typedef void(*WindowCharCallback)(Window*, uint32_t);
    typedef void(*WindowCharModsCallback)(Window*, uint32_t, int32_t);
    typedef void(*WindowDropCallback)(Window*, uint32_t, const char**);

    struct WindowConfig
    {
        std::string title;
        int32_t width;
        int32_t height;
        bool decorated;
        bool visible;
        bool focused;
        bool focusOnShow;
        bool autoIconify;
        bool floating;
        bool maximized;
        bool resizable;
        bool centerCursor;
        bool mousePassthrough;
        bool scaleToMonitor;
        int32_t refreshRate;

        //win32 only
        bool keyMenu;
    };

    struct FramebufferConfig
    {
        int32_t redBits;
        int32_t greenBits;
        int32_t blueBits;
        int32_t alphaBits;
        int32_t depthBits;
        int32_t stencilBits;
        int32_t accumRedBits;
        int32_t accumGreenBits;
        int32_t accumBlueBits;
        int32_t accumAlphaBits;
        int32_t auxBuffers;
        bool stereo;
        int samples;
        bool sRGB;
        bool doubleBuffer;
        bool transparent;
        uintptr_t handle;
    };

    class Window
    {
    protected:
        std::string m_Title = {};
        int32_t m_Width = 0; //cached used to filter out duplicate events
        int32_t m_Height = 0; //cached used to filter out duplicate events
        int32_t m_MinWidth = -1;
        int32_t m_MinHeight = -1;
        int32_t m_MaxWidth = -1;
        int32_t m_MaxHeight = -1;
        bool m_Floating = false;
        bool m_Decorated = false;
        bool m_Resizable = false;
        bool m_FocusOnShow = false;
        bool m_AutoMinimize = false;
        bool m_MousePassthrough = false;
        bool m_ShouldClose = false;
        int32_t m_Numerator = -1;
        int32_t m_Denominator = -1;

        int32_t m_CursorMode = 0;
        int8_t m_MouseButtons[DAIS_MOUSE_BUTTON_LAST + 1] = {};
        int8_t m_Keys[DAIS_KEY_LAST + 1] = {};
        bool m_StickyKeys = false;
        bool m_StickyMouseButtons = false;
        bool m_LockKeyMods = false;
        bool m_RawMouseMotion = false;
        double m_VirtualCursorPositionX = 0.0;
        double m_VirtualCursorPositionY = 0.0;

        VideoMode m_VideoMode = {};
        Monitor* m_Monitor = nullptr;
        Cursor* m_Cursor = nullptr;

        struct WindowCallbacks
        {
            WindowPositionCallback position;
            WindowSizeCallback size;
            WindowCloseCallback close;
            WindowRefreshCallback refresh;
            WindowFocusCallback focus;
            WindowMinimizeCallback minimize;
            WindowMaximizeCallback maximize;
            WindowFramebufferSizeCallback framebufferSize;
            WindowContentScaleCallback contentScale;
            WindowMouseButtonCallback mouseButton;
            WindowCursorPositionCallback cursorPosition;
            WindowCursorEnterCallback cursorEnter;
            WindowScrollCallback scroll;
            WindowKeyCallback key;
            WindowCharCallback character;
            WindowCharModsCallback characterMods;
            WindowDropCallback drop;
        } m_Callbacks = {};

    protected:
        Window(WindowConfig config, FramebufferConfig fbConfig, Monitor* monitor);

    public:
        static Window* Create(WindowConfig config, FramebufferConfig fbConfig, Monitor* monitor);
        static Window* PlatformCreate(WindowConfig config, FramebufferConfig fbConfig, Monitor* monitor);

    public:
        virtual ~Window();

    public:
        bool IsMaximized() const;
        bool IsMinimized() const;
        bool IsVisible() const;
        bool IsFocused() const;
        bool IsHovered() const;
        bool IsFloating() const;
        bool IsDecorated() const;
        bool IsResizable() const;
        bool IsFocusOnShow() const;
        bool IsFramebufferTransparent() const;
        bool IsAutoIconify() const;
        bool IsMousePassThrough() const;
        bool ShouldClose() const; // Close icon pressed

        const std::string& GetTitle() const;
        void GetPosition(int32_t* x, int32_t* y) const;
        void GetSize(int32_t* width, int32_t* height);
        void GetFramebufferSize(int32_t* width, int32_t* height) const;
        void GetFrameSize(int32_t* left, int32_t* top, int32_t* right, int32_t* bottom);
        void GetContentScale(float* xScale, float* yScale);
        float GetOpacity();
        const Monitor* GetMonitor() const;
        int32_t GetInputMode(int32_t mode); //DAIS_CURSOR_NORMAL, etc or true false
        int32_t GetKey(int32_t key);
        int32_t GetGetMouseButton(int32_t button);
        void GetCursorPosition(double* x, double* y);
        void* GetNativeHandle() const;

        void SetTitle(const std::string& title);
        void SetIcon(const std::vector<Image*>& images);
        void SetCursorType(Cursor* cursor);
        void SetPosition(int32_t x, int32_t y);
        void SetSize(int32_t width, int32_t height);
        void SetSizeLimits(int32_t minWidth, int32_t minHeight, int32_t maxWidth, int32_t maxHeight);
        void SetAspectRatio(int32_t numerator, int32_t denominator);
        void SetOpacity(float opacity);
        void SetFloating(bool value);
        void SetDecorated(bool value);
        void SetResizable(bool value);
        void SetFocusOnShow(bool value);
        void SetAutoIconify(bool value);
        void SetMousePassThrough(bool value);
        void SetShouldClose(bool value);
        void SetMonitor(Monitor* monitor, int32_t x, int32_t y, int32_t width, int32_t height, int32_t refreshRate);
        void SetInputMode(int32_t mode, int32_t value);
        void SetCursorPosition(double x, double y);

        void Maximize();
        void Minimize();
        void Restore();
        void Show();
        void Hide();
        void Focus();
        void RequestAttention();
        void CenterCursorInContentArea();

        void SetPositionCallback(WindowPositionCallback callback);
        void SetSizeCallback(WindowSizeCallback callback);
        void SetCloseCallback(WindowCloseCallback callback);
        void SetRefreshCallback(WindowRefreshCallback callback);
        void SetFocusCallback(WindowFocusCallback callback);
        void SetMinimizeCallback(WindowMinimizeCallback callback);
        void SetMaximizeCallback(WindowMaximizeCallback callback);
        void SetFramebufferSizeCallback(WindowFramebufferSizeCallback callback);
        void SetContentScaleCallback(WindowContentScaleCallback callback);

        void SetKeyCallback(WindowKeyCallback callback);
        void SetCharCallback(WindowCharCallback callback);
        void SetCharModsCallback(WindowCharModsCallback callback);
        void SetMouseButtonCallback(WindowMouseButtonCallback callback);
        void SetCursorPositionCallback(WindowCursorPositionCallback callback);
        void SetCursorEnterCallback(WindowCursorEnterCallback callback);
        void SetScrollCallback(WindowScrollCallback callback);
        void SetDropCallback(WindowDropCallback callback);

    protected:
        const Image* ChooseImage(const std::vector<Image*>& images, int32_t width, int32_t height);

    protected:
        virtual bool PlatformIsMaximized() const = 0;
        virtual bool PlatformIsMinimized() const = 0;
        virtual bool PlatformIsVisible() const = 0;
        virtual bool PlatformIsHovered() const = 0;
        virtual bool PlatformIsFocused() const = 0;
        virtual bool PlatformIsFramebufferTransparent() const = 0;

        virtual void PlatformGetPosition(int32_t* x, int32_t* y) const = 0;
        virtual void PlatformGetSize(int32_t* width, int32_t* height) const = 0;
        virtual void PlatformGetFramebufferSize(int32_t* width, int32_t* height) const = 0;
        virtual void PlatformGetFrameSize(int32_t* left, int32_t* top, int32_t* right, int32_t* bottom) const = 0;
        virtual void PlatformGetContentScale(float* xScale, float* yScale) = 0;
        virtual float PlatformGetOpacity() = 0;
        virtual void PlatformGetCursorPosition(double* x, double* y) = 0;
        virtual const char* PlatformGetClipboardString() = 0;
        virtual void* PlatformGetHandle() const = 0;

        virtual void PlatformSetTitle(const std::string& title) = 0;
        virtual void PlatformSetIcon(const std::vector<Image*>& images) = 0;
        virtual void PlatformSetCursorType(Cursor* cursor) = 0;
        virtual void PlatformSetPosition(int32_t x, int32_t y) = 0;
        virtual void PlatformSetSize(int32_t width, int32_t height) = 0;
        virtual void PlatformSetSizeLimits(int32_t minWidth, int32_t minHeight, int32_t maxWidth, int32_t maxHeight) = 0;
        virtual void PlatformSetAspectRatio(int32_t numerator, int32_t denominator) = 0;
        virtual void PlatformSetOpacity(float opacity) = 0;
        virtual void PlatformSetCursor(Cursor* cursor) = 0;
        virtual void PlatformSetCursorPosition(double x, double y) = 0;
        virtual void PlatformSetCursorMode(int32_t mode) = 0;
        virtual void PlatformSetRawMouseMotion(bool enabled) = 0;
        virtual void PlatformSetDecorated(bool value) = 0;
        virtual void PlatformSetFloating(bool value) = 0;
        virtual void PlatformSetResizable(bool value) = 0;
        virtual void PlatformSetMousePassThrough(bool value) = 0;
        virtual void PlatformSetClipboardString(const char* string) = 0;
        virtual void PlatformSetMonitor(Monitor* monitor, int32_t x, int32_t y, int32_t width, int32_t height, int32_t refreshRate) = 0;

        virtual void PlatformMaximize() = 0;
        virtual void PlatformMinimize() = 0;
        virtual void PlatformRestore() = 0;
        virtual void PlatformShow() = 0;
        virtual void PlatformHide() = 0;
        virtual void PlatformRequestAttention() = 0;
        virtual void PlatformFocus() = 0;

    protected:
        void OnPositionChanged(int32_t x, int32_t y);
        void OnSizeChanged(int32_t width, int32_t height);
        void OnClosed();
        void OnNeedUpdate();
        void OnFocus(bool focused);
        void OnMinimize(bool minimized);
        void OnMaximize(bool maximized);
        void OnFramebufferSizeChanged(int32_t width, int32_t height);
        void OnContentScaleChanged(float xScale, float yScale);
        void OnMouseButton(int32_t button, int32_t action, int32_t mods);
        void OnCursorPositionChanged(double x, double y);
        void OnCursorEnter(bool entered);
        void OnScroll(double xOffset, double yOffset);
        void OnKey(int32_t key, int32_t scancode, int32_t action, int32_t mods);
        void OnChar(uint32_t codepoint, int32_t mods, bool plain);
        void OnCharMods(uint32_t codepoint, int32_t mods);
        void OnDrop(uint32_t count, const char** paths);
    };
}
