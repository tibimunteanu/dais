#include "engine/core/Platform.h"

namespace dais
{
    ////////////////////////////////////// STATIC INIT ////////////////////////////////////////

    EglContext::EglLib EglContext::s_EGL = {};



    ///////////////////////////////////// INTERNAL API ////////////////////////////////////////

    bool EglContext::Init()
    {
        if (s_EGL.handle)
        {
            return true;
        }

        const std::vector<std::string>& libNames = Platform::GetEglLibNames();
        for (const std::string& libName : libNames)
        {
            s_EGL.handle = Platform::OpenLibrary(libName);
            if (s_EGL.handle)
            {
                break;
            }
        }

        if (!s_EGL.handle)
        {
            DAIS_ERROR("Could not load EGL library!");
            return false;
        }

        return true;
    }

    void EglContext::Terminate()
    {
    }


    bool EglContext::CreateContext(Window* window, const ContextConfig* contextConfig, const FramebufferConfig* framebufferConfig)
    {
        return false;
    }
}
