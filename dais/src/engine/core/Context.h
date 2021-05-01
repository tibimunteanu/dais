#pragma once

#include "engine/core/Base.h"

namespace dais
{
    class Window;
    struct FramebufferConfig;
    struct ContextConfig;

    class Context
    {
    public:
        ContextAPI m_API;
        ContextType m_Type;
        int32_t m_Major;
        int32_t m_Minor;
        int32_t m_Revision;
        bool m_Forward;
        bool m_Debug;
        bool m_NoError;
        ContextProfile m_Profile;
        ContextRobustnessMode m_Robustness;
        ContextReleaseBehavior m_Release;

        //common OpenGL function pointers
        PFNGLGETSTRINGIPROC GetStringi;
        PFNGLGETINTEGERVPROC GetIntegerv;
        PFNGLGETSTRINGPROC GetString;

    public: DAIS_PUBLIC_API
        static Window* GetCurrentContext();
        static bool StringInExtensionString(const char* string, const char* extensions);
        static const FramebufferConfig* ChooseFramebufferConfig(const FramebufferConfig* desired, const std::vector<FramebufferConfig>& alternatives);
        static bool RefreshContextAttribs(Window* window, const ContextConfig* contextConfig);

        static void MakeContextCurrent(Window* window);
        static void SwapBuffers(Window* window);
        static void SwapInterval(int32_t interval);
        static bool ExtensionSupported(const char* extension);
        static GLProc GetGLProcAddress(const char* procedureName);
        static void DestroyContext(Window* window);

    protected: DAIS_PLATFORM_API
        static void PlatformMakeContextCurrent(Window* window);
        static void PlatformSwapBuffers(Window* window);
        static void PlatformSwapInterval(int32_t interval);
        static bool PlatformExtensionSupported(const char* extension);
        static GLProc PlatformGetGLProcAddress(const char* procedureName);
        static void PlatformDestroyContext(Window* window);
    };
}
