#pragma once

#include "platform/windows/WindowsBase.h"

namespace dais
{
    class WindowsWindow;

    class WindowsWglContext : public Context
    {
    public:
        static struct WglLib
        {
            HINSTANCE instance;

            //core wgl function poinnters
            PFNWGLCREATECONTEXTPROC createContext;
            PFNWGLDELETECONTEXTPROC deleteContext;
            PFNWGLGETPROCADDRESSPROC getProcAddress;
            PFNWGLGETCURRENTDCPROC getCurrentDC;
            PFNWGLGETCURRENTCONTEXTPROC getCurrentContext;
            PFNWGLMAKECURRENTPROC makeCurrent;
            PFNWGLSHARELISTSPROC shareLists;

            //function pointers for common extensions
            PFNWGLSWAPINTERVALEXTPROC swapIntervalEXT;
            PFNWGLGETPIXELFORMATATTRIBIVARBPROC getPixelFormatAttribivARB;
            PFNWGLGETEXTENSIONSSTRINGEXTPROC getExtensionsStringEXT;
            PFNWGLGETEXTENSIONSSTRINGARBPROC getExtensionsStringARB;
            PFNWGLCREATECONTEXTATTRIBSARBPROC createContextAttribsARB;

            //common extensions support
            bool EXT_SwapControl;
            bool EXT_Colorspace;
            bool ARB_Multisample;
            bool ARB_FramebufferSRGB;
            bool EXT_FramebufferSRGB;
            bool ARB_PixelFormat;
            bool ARB_CreateContext;
            bool ARB_CreateContextProfile;
            bool EXT_CreateContextES2Profile;
            bool ARB_CreateContextRobustness;
            bool ARB_CreateContextNoError;
            bool ARB_ContextFlushControl;
        } s_WGL;

    public:
        HDC m_DC;
        HGLRC m_Handle;
        int32_t m_Interval;

    public: DAIS_INTERNAL_API
        static bool Init();
        static void Terminate();

        static bool CreateContext(WindowsWindow* window, const ContextConfig* contextConfig, const FramebufferConfig* framebufferConfig);

    private: DAIS_UTILS
        static int32_t GetClosestPixelFormat(HDC dc, const ContextConfig* contextConfig, const FramebufferConfig* framebufferConfig);
    };
}
