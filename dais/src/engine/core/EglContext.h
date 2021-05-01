#pragma once

#include "engine/core/Base.h"

namespace dais
{
    class EglContext : public Context
    {
    public:
        static struct EglLib
        {
            EGLenum platform;
            EGLDisplay display;
            EGLint major;
            EGLint minor;
            bool prefix;

            bool KHR_CreateContext;
            bool KHR_CreateContextNoError;
            bool KHR_GlColorspace;
            bool KHR_GetAllProcAddresses;
            bool KHR_ContextFlushControl;
            bool EXT_ClientExtensions;
            bool EXT_PlatformBase;
            bool EXT_PlatformX11;
            bool EXT_PlatformWayland;
            bool ANGLE_PlatformAngle;
            bool ANGLE_PlatformAngleOpenGL;
            bool ANGLE_PlatformAngleD3D;
            bool ANGLE_PlatformAngleVulkan;
            bool ANGLE_PlatformAngleMetal;

            void* handle;

            PFN_eglGetConfigAttrib getConfigAttrib;
            PFN_eglGetConfigs getConfigs;
            PFN_eglGetDisplay getDisplay;
            PFN_eglGetError getError;
            PFN_eglInitialize initialize;
            PFN_eglTerminate terminate;
            PFN_eglBindAPI bindAPI;
            PFN_eglCreateContext createContext;
            PFN_eglDestroySurface destroySurface;
            PFN_eglDestroyContext destroyContext;
            PFN_eglCreateWindowSurface createWindowSurface;
            PFN_eglMakeCurrent makeCurrent;
            PFN_eglSwapBuffers swapBuffers;
            PFN_eglSwapInterval swapInterval;
            PFN_eglQueryString queryString;
            PFN_eglGetProcAddress getProcAddress;

            PFNEGLGETPLATFORMDISPLAYEXTPROC getPlarformDisplayEXT;
            PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC createPlatformWindowSurfaceEXT;
        } s_EGL;

    public:
        EGLConfig m_Config;
        EGLContext m_Handle;
        EGLSurface m_Surface;
        void* m_Client;

    public: DAIS_INTERNAL_API
        static bool Init();
        static void Terminate();

        static bool CreateContext(Window* window, const ContextConfig* contextConfig, const FramebufferConfig* framebufferConfig);
    };
}
