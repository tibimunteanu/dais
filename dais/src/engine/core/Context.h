#pragma once

#include "engine/core/Base.h"

namespace dais
{
    class Window;
    struct FramebufferConfig;
    struct ContextConfig;

    typedef void (*GLProc)(void);

    typedef void (*MakeContextCurrentFun)(Window*);
    typedef void (*SwapBuffersFun)(Window*);
    typedef void (*SwapIntervalFun)(int);
    typedef bool (*ExtensionSupportedFun)(const char*);
    typedef GLProc(*GetProcAddressFun)(const char*);
    typedef void (*DestroyContextFun)(Window*);

    typedef void (APIENTRY* PFNGLCLEARPROC)(GLbitfield);
    typedef const GLubyte* (APIENTRY* PFNGLGETSTRINGPROC)(GLenum);
    typedef void (APIENTRY* PFNGLGETINTEGERVPROC)(GLenum, GLint*);
    typedef const GLubyte* (APIENTRY* PFNGLGETSTRINGIPROC)(GLenum, GLuint);

    class Context
    {
    public:
        int32_t m_Client;
        int32_t m_Source;
        int32_t m_Major;
        int32_t m_Minor;
        int32_t m_Revision;
        bool m_Forward;
        bool m_Debug;
        bool m_NoError;
        int32_t m_Profile;
        int32_t m_Robustness;
        int32_t m_Release;

        PFNGLGETSTRINGIPROC GetStringi;
        PFNGLGETINTEGERVPROC GetIntegerv;
        PFNGLGETSTRINGPROC GetString;

        MakeContextCurrentFun m_MakeCurrent;
        SwapBuffersFun m_SwapBuffers;
        SwapIntervalFun m_SwapInterval;
        ExtensionSupportedFun m_ExtensionSupported;
        GetProcAddressFun m_GetProcAddress;
        DestroyContextFun m_Destroy;

    public: DAIS_PUBLIC_API
        static bool StringInExtensionString(const char* string, const char* extensions);
        static const FramebufferConfig* ChooseFramebufferConfig(const FramebufferConfig* desired, const std::vector<FramebufferConfig>& alternatives);
        static bool RefreshContextAttribs(Window* window, const ContextConfig* contextConfig);
        static void MakeContextCurrentGL(Window* window);
        static Window* GetCurrentContextGL();
        static void SwapBuffersGL(Window* window);
        static void SwapIntervalGL(int32_t interval);
        static bool ExtensionSupportedGL(const char* extension);
        static GLProc GetProcAddressGL(const char* procedureName);
    };
}
