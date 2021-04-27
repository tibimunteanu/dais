#pragma once

#include "engine/core/Base.h"

namespace dais
{
    class Window;
    struct FramebufferConfig;
    struct ContextConfig;

    typedef void (*GLProc)(void);

    //TEMP: function pointers for public api
    typedef void (*MakeContextCurrentFun)(Window*);
    typedef void (*SwapBuffersFun)(Window*);
    typedef void (*SwapIntervalFun)(int);
    typedef bool (*ExtensionSupportedFun)(const char*);
    typedef GLProc(*GetProcAddressFun)(const char*);
    typedef void (*DestroyContextFun)(Window*);

    //function pointer typedefs from glcorearb.h
    typedef void (APIENTRY* PFNGLCLEARPROC)(GLbitfield mask);
    typedef const GLubyte* (APIENTRY* PFNGLGETSTRINGPROC)(GLenum name);
    typedef void (APIENTRY* PFNGLGETINTEGERVPROC)(GLenum pname, GLint* data);
    typedef const GLubyte* (APIENTRY* PFNGLGETSTRINGIPROC)(GLenum name, GLuint index);

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

        static Window* GetCurrentContextGL();
        static void MakeContextCurrentGL(Window* window);
        static void SwapBuffersGL(Window* window);
        static void SwapIntervalGL(int32_t interval);
        static bool ExtensionSupportedGL(const char* extension);
        static GLProc GetProcAddressGL(const char* procedureName);
    };
}
