#pragma once

#include "platform/windows/WindowsBase.h"

namespace dais
{
    class WindowsWindow;

    class WglContext : public Context
    {
    public:
        static struct WglLib
        {
            HINSTANCE instance;
            PFN_wglCreateContext createContext;
            PFN_wglDeleteContext deleteContext;
            PFN_wglGetProcAddress getProcAddress;
            PFN_wglGetCurrentDC getCurrentDC;
            PFN_wglGetCurrentContext getCurrentContext;
            PFN_wglMakeCurrent makeCurrent;
            PFN_wglShareLists shareLists;

            PFNWGLSWAPINTERVALEXTPROC swapIntervalEXT;
            PFNWGLGETPIXELFORMATATTRIBIVARBPROC getPixelFormatAttribivARB;
            PFNWGLGETEXTENSIONSSTRINGEXTPROC getExtensionsStringEXT;
            PFNWGLGETEXTENSIONSSTRINGARBPROC getExtensionsStringARB;
            PFNWGLCREATECONTEXTATTRIBSARBPROC createContextAttribsARB;

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

    public:
        static bool InitWGL();
        static void TerminateWGL();
        static bool CreateContextWGL(WindowsWindow* window, const ContextConfig* contextConfig, const FramebufferConfig* framebufferConfig);
        static int32_t FindPixelFormatAttribValue(const int32_t* attribs, int32_t attribCount, const int32_t* values, int32_t attrib);
        static int32_t ChoosePixelFormatWGL(Window* window, const ContextConfig* contextConfig, const FramebufferConfig* framebufferConfig);
        static void MakeContextCurrentWGL(Window* window);
        static void SwapBuffersWGL(Window* window);
        static void SwapIntervalWGL(int32_t interval);
        static bool ExtensionSupportedWGL(const char* extension);
        static GLProc GetProcAddressWGL(const char* procedureName);
        static void DestroyContextWGL(Window* window);
    };
}
