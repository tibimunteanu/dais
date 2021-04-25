#include "engine/core/Platform.h"

namespace dais
{
    ///////////////////////////////////// PUBLIC STATIC API ///////////////////////////////////////

    bool Context::StringInExtensionString(const char* string, const char* extensions)
    {
        const char* start = extensions;

        for (;;)
        {
            const char* where = strstr(start, string);
            if (!where)
            {
                return false;
            }

            const char* terminator = where + strlen(string);
            if (where == start
                || *(where - 1) == ' ')
            {
                if (*terminator == ' '
                    || *terminator == '\0')
                {
                    break;
                }
            }

            start = terminator;
        }

        return true;
    }

    const FramebufferConfig* Context::ChooseFramebufferConfig(const FramebufferConfig* desired, const std::vector<FramebufferConfig>& alternatives)
    {
        uint32_t missing, leastMissing = UINT_MAX;
        uint32_t colorDiff, leastColorDiff = UINT_MAX;
        uint32_t extraDiff, leastExtraDiff = UINT_MAX;

        const FramebufferConfig* current;
        const FramebufferConfig* closest = nullptr;

        for (uint32_t i = 0; i < alternatives.size(); i++)
        {
            current = &alternatives[i];

            if (desired->stereo > 0
                && current->stereo == 0)
            {
                //stereo is a hard constraint
                continue;
            }

            if (desired->doubleBuffer != current->doubleBuffer)
            {
                //double buffering is a hard constraint
                continue;
            }

            //count number of missing buffers
            {
                missing = 0;
                if (desired->alphaBits > 0
                    && current->alphaBits == 0)
                {
                    missing++;
                }

                if (desired->depthBits > 0
                    && current->depthBits == 0)
                {
                    missing++;
                }

                if (desired->stencilBits > 0
                    && current->stencilBits == 0)
                {
                    missing++;
                }

                if (desired->auxBuffers > 0
                    && current->auxBuffers < desired->auxBuffers)
                {
                    missing += desired->auxBuffers - current->auxBuffers;
                }

                if (desired->samples > 0
                    && current->samples == 0)
                {
                    //technically, several multisampling buffers could be
                    //involved, but that's a lower level implementation detail
                    //and not important to us here, so we count them as one
                    missing++;
                }

                if (desired->transparent != current->transparent)
                {
                    missing++;
                }
            }

            //these polynomials make many small channel size differences matter
            //less than one large channel size difference

            //calculate color channel size difference value
            {
                colorDiff = 0;

                if (desired->redBits != -1)
                {
                    int32_t diff = desired->redBits - current->redBits;
                    colorDiff += diff * diff;
                }

                if (desired->greenBits != -1)
                {
                    int32_t diff = desired->greenBits - current->greenBits;
                    colorDiff += diff * diff;
                }

                if (desired->blueBits != -1)
                {
                    int32_t diff = desired->blueBits - current->blueBits;
                    colorDiff += diff * diff;
                }
            }

            //calculate non-color channel size difference value
            {
                extraDiff = 0;

                if (desired->alphaBits != -1)
                {
                    int32_t diff = desired->alphaBits - current->alphaBits;
                    extraDiff += diff * diff;
                }

                if (desired->depthBits != -1)
                {
                    int32_t diff = desired->depthBits - current->depthBits;
                    extraDiff += diff * diff;
                }

                if (desired->stencilBits != -1)
                {
                    int32_t diff = desired->stencilBits - current->stencilBits;
                    extraDiff += diff * diff;
                }

                if (desired->accumRedBits != -1)
                {
                    int32_t diff = desired->accumRedBits - current->accumRedBits;
                    extraDiff += diff * diff;
                }

                if (desired->accumGreenBits != -1)
                {
                    int32_t diff = desired->accumGreenBits - current->accumGreenBits;
                    extraDiff += diff * diff;
                }

                if (desired->accumBlueBits != -1)
                {
                    int32_t diff = desired->accumBlueBits - current->accumBlueBits;
                    extraDiff += diff * diff;
                }

                if (desired->accumAlphaBits != -1)
                {
                    int32_t diff = desired->accumAlphaBits - current->accumAlphaBits;
                    extraDiff += diff * diff;
                }

                if (desired->samples != -1)
                {
                    int32_t diff = desired->samples - current->samples;
                    extraDiff += diff * diff;
                }

                if (desired->sRGB
                    && !current->sRGB)
                {
                    extraDiff++;
                }
            }

            //figure out if the current one is better than the best one found so far
            //least number of missing buffers is the most important heuristic,
            //then color buffer size mtch and lastly size match for other buffers

            if (missing < leastMissing)
            {
                closest = current;
            }
            else if (missing == leastMissing)
            {
                if ((colorDiff < leastColorDiff)
                    || (colorDiff == leastColorDiff && extraDiff < leastExtraDiff))
                {
                    closest = current;
                }
            }

            if (current == closest)
            {
                leastMissing = missing;
                leastColorDiff = colorDiff;
                leastExtraDiff = extraDiff;
            }
        }

        return closest;
    }

    bool Context::RefreshContextAttribs(Window* window, const ContextConfig* contextConfig)
    {
        const char* prefixes[] =
        {
            "OpenGL ES-CM ",
            "OpenGL ES-CL ",
            "OpenGL ES ",
            nullptr
        };

        window->GetContext()->m_Source = contextConfig->source;
        window->GetContext()->m_Client = DAIS_OPENGL_API;

        Window* previous = GetCurrentContextGL();
        MakeContextCurrentGL(window);

        window->GetContext()->GetIntegerv = (PFNGLGETINTEGERVPROC)window->GetContext()->m_GetProcAddress("glGetIntegerv");
        window->GetContext()->GetString = (PFNGLGETSTRINGPROC)window->GetContext()->m_GetProcAddress("glGetString");
        if (!window->GetContext()->GetIntegerv
            || !window->GetContext()->GetString)
        {
            DAIS_ERROR("Entry point retrieval is broken!");
            MakeContextCurrentGL(previous);
            return false;
        }

        const char* version = (const char*)window->GetContext()->GetString(GL_VERSION);
        if (!version)
        {
            if (contextConfig->client == DAIS_OPENGL_API)
            {
                DAIS_ERROR("OpenGL version string retrieval is broken!");
            }
            else
            {
                DAIS_ERROR("OpenGL ES version string retrieval is broken!");
            }
            MakeContextCurrentGL(previous);
            return false;
        }

        for (int32_t i = 0; prefixes[i]; i++)
        {
            const size_t length = strlen(prefixes[i]);

            if (strncmp(version, prefixes[i], length) == 0)
            {
                version += length;
                window->GetContext()->m_Client = DAIS_OPENGL_ES_API;
                break;
            }
        }

        if (!sscanf(version, "%d.%d.%d",
            &window->GetContext()->m_Major,
            &window->GetContext()->m_Minor,
            &window->GetContext()->m_Revision))
        {
            if (window->GetContext()->m_Client == DAIS_OPENGL_API)
            {
                DAIS_ERROR("No version found on OpenGL version string!");
            }
            else
            {
                DAIS_ERROR("No version found on OpenGL ES version string!");
            }
            MakeContextCurrentGL(previous);
            return false;
        }

        if (window->GetContext()->m_Major < contextConfig->major
            || (window->GetContext()->m_Major == contextConfig->major && window->GetContext()->m_Minor < contextConfig->minor))
        {
            //the desired OpenGL version is greater than the actual version
            //this only happens if the machine lacks {GLX|WGL}_ARB_create_context
            //and the user has requested an OpenGL version greater than 1.0

            //for API consistency, we emulate the behavior of the
            //{GLX|WGL}_ARB_create_context extension and fail here

            if (window->GetContext()->m_Client == DAIS_OPENGL_API)
            {
                DAIS_ERROR("Requested OpenGL version %i.%i, got version %i.%i!",
                    contextConfig->major, contextConfig->minor,
                    window->GetContext()->m_Major, window->GetContext()->m_Minor);
            }
            else
            {
                DAIS_ERROR("Requested OpenGL ES version %i.%i, got version %i.%i!",
                    contextConfig->major, contextConfig->minor,
                    window->GetContext()->m_Major, window->GetContext()->m_Minor);
            }
            MakeContextCurrentGL(previous);
            return false;
        }

        if (window->GetContext()->m_Major >= 3)
        {
            //OpenGL 3.0+ uses a different function for extension string retrieval
            //we cache it here instead of in ExtensionSupported mostly to alert
            //users as early as possible that their build may be broken

            window->GetContext()->GetStringi = (PFNGLGETSTRINGIPROC)window->GetContext()->m_GetProcAddress("glGetStringi");
            if (!window->GetContext()->GetStringi)
            {
                DAIS_ERROR("Entry point retrieval is broken!");
                MakeContextCurrentGL(previous);
                return false;
            }
        }

        if (window->GetContext()->m_Client == DAIS_OPENGL_API)
        {
            //read back context flags (OpenGL 3.0+)
            if (window->GetContext()->m_Major >= 3)
            {
                GLint flags;
                window->GetContext()->GetIntegerv(GL_CONTEXT_FLAGS, &flags);

                if (flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT)
                {
                    window->GetContext()->m_Forward = true;
                }

                if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
                {
                    window->GetContext()->m_Debug = true;
                }
                else if (ExtensionSupportedGL("GL_ARB_debug_output")
                    && contextConfig->debug)
                {
                    //HACK: this is a workaround for older drivers (pre KHR_debug)
                    // not setting the debug bit in the context flags for debug contexts
                    window->GetContext()->m_Debug = true;
                }

                if (flags & GL_CONTEXT_FLAG_NO_ERROR_BIT_KHR)
                {
                    window->GetContext()->m_NoError = true;
                }
            }

            //read back OpenGL context profile (OpenGL 3.2+)
            if (window->GetContext()->m_Major >= 4
                || (window->GetContext()->m_Major == 3 && window->GetContext()->m_Minor >= 2))
            {
                GLint mask;
                window->GetContext()->GetIntegerv(GL_CONTEXT_PROFILE_MASK, &mask);

                if (mask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)
                {
                    window->GetContext()->m_Profile = DAIS_OPENGL_COMPAT_PROFILE;
                }
                else if (mask & GL_CONTEXT_CORE_PROFILE_BIT)
                {
                    window->GetContext()->m_Profile = DAIS_OPENGL_CORE_PROFILE;
                }
                else if (ExtensionSupportedGL("GL_ARB_compatibility"))
                {
                    //HACK: this is a workaround for the compatibility profile bit
                    //not being set in the context flags in an OpenGL 3.2+ context
                    //was created without having requested a specific version
                    window->GetContext()->m_Profile = DAIS_OPENGL_COMPAT_PROFILE;
                }
            }

            //read back robustness strategy
            if (ExtensionSupportedGL("GL_ARB_robustness"))
            {
                //NOTE: we avoid using the context flags for detection, as they are
                //only present from 3.0 while the extension applies from 1.1

                GLint strategy;
                window->GetContext()->GetIntegerv(GL_RESET_NOTIFICATION_STRATEGY_ARB, &strategy);

                if (strategy == GL_LOSE_CONTEXT_ON_RESET_ARB)
                {
                    window->GetContext()->m_Robustness = DAIS_LOSE_CONTEXT_ON_RESET;
                }
                else if (strategy == GL_NO_RESET_NOTIFICATION_ARB)
                {
                    window->GetContext()->m_Robustness = DAIS_NO_RESET_NOTIFICATION;
                }
            }
        }
        else
        {
            //read back robustness strategy
            if (ExtensionSupportedGL("GL_EXT_robustness"))
            {
                //NOTE: the values of these constants match those of the OpenGL ARB one,
                //so we can reuse them here

                GLint strategy;
                window->GetContext()->GetIntegerv(GL_RESET_NOTIFICATION_STRATEGY_ARB, &strategy);

                if (strategy == GL_LOSE_CONTEXT_ON_RESET_ARB)
                {
                    window->GetContext()->m_Robustness = DAIS_LOSE_CONTEXT_ON_RESET;
                }
                else if (strategy == GL_NO_RESET_NOTIFICATION_ARB)
                {
                    window->GetContext()->m_Robustness = DAIS_NO_RESET_NOTIFICATION;
                }
            }
        }

        if (ExtensionSupportedGL("GL_KHR_context_flush_control"))
        {
            GLint behavior;
            window->GetContext()->GetIntegerv(GL_CONTEXT_RELEASE_BEHAVIOR, &behavior);

            if (behavior == GL_NONE)
            {
                window->GetContext()->m_Release = DAIS_RELEASE_BEHAVIOR_NONE;
            }
            else if (behavior == GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH)
            {
                window->GetContext()->m_Release = DAIS_RELEASE_BEHAVIOR_FLUSH;
            }
        }

        //clearing the front buffer to black to avoid garbage pixels left over from
        //previous uses of our bit of VRAM
        {
            PFNGLCLEARPROC glClear = (PFNGLCLEARPROC)window->GetContext()->m_GetProcAddress("glClear");
            glClear(GL_COLOR_BUFFER_BIT);
            window->GetContext()->m_SwapBuffers(window);
        }

        MakeContextCurrentGL(previous);
        return true;
    }

    void Context::MakeContextCurrentGL(Window* window)
    {
        Window* previous = GetCurrentContextGL();

        if (window
            && window->GetContext()->m_Client == DAIS_NO_API)
        {
            DAIS_ERROR("Cannot make current with a window that has no OpenGL or OpenGL ES context!");
            return;
        }

        if (previous)
        {
            if (!window
                || window->GetContext()->m_Source != previous->GetContext()->m_Source)
            {
                previous->GetContext()->m_MakeCurrent(nullptr);
            }
        }

        if (window)
        {
            window->GetContext()->m_MakeCurrent(window);
        }
    }

    Window* Context::GetCurrentContextGL()
    {
        return (Window*)Platform::s_ContextSlot->Get();
    }

    void Context::SwapBuffersGL(Window* window)
    {
        if (window->GetContext()->m_Client == DAIS_NO_API)
        {
            DAIS_ERROR("Cannot swap buffers of a window that has no OpenGL or OpenGL ES context!");
            return;
        }

        window->GetContext()->m_SwapBuffers(window);
    }

    void Context::SwapIntervalGL(int32_t interval)
    {
        Window* window = GetCurrentContextGL();
        if (!window)
        {
            DAIS_ERROR("Cannot set swap interval without a current OpenGL or OpenGL ES context!");
            return;
        }

        window->GetContext()->m_SwapInterval(interval);
    }

    bool Context::ExtensionSupportedGL(const char* extension)
    {
        Window* window = GetCurrentContextGL();
        if (!window)
        {
            DAIS_ERROR("Cannot query extension without a current OpenGL or OpenGL ES context!");
            return false;
        }

        if (*extension == '\0')
        {
            DAIS_ERROR("Extension name cannot be empty!");
            return false;
        }

        if (window->GetContext()->m_Major >= 3)
        {
            //check if extension is in the modern OpenGL extensions string list
            GLint count;
            window->GetContext()->GetIntegerv(GL_NUM_EXTENSIONS, &count);

            for (int32_t i = 0; i < count; i++)
            {
                const char* en = (const char*)window->GetContext()->GetStringi(GL_EXTENSIONS, i);
                if (!en)
                {
                    DAIS_ERROR("Extension string retrieval is broken!");
                    return false;
                }

                if (strcmp(en, extension) == 0)
                {
                    return true;
                }
            }
        }
        else
        {
            //check if extension is in the old style OpenGL extension string
            const char* extensions = (const char*)window->GetContext()->GetString(GL_EXTENSIONS);
            if (!extensions)
            {
                DAIS_ERROR("Extension string retrieval is broken!");
                return false;
            }

            if (StringInExtensionString(extension, extensions))
            {
                return true;
            }
        }

        //check if extension is in the platform-specific string
        return window->GetContext()->m_ExtensionSupported(extension);
    }

    GLProc Context::GetProcAddressGL(const char* procedureName)
    {
        Window* window = GetCurrentContextGL();
        if (!window)
        {
            DAIS_ERROR("Cannot query entry point without a current OpenGL or OpenGL ES context!");
            return nullptr;
        }

        return window->GetContext()->m_GetProcAddress(procedureName);
    }
}
