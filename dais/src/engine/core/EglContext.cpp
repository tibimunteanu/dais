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
                s_EGL.prefix = libName.find("lib") == 0;
                break;
            }
        }

        if (!s_EGL.handle)
        {
            DAIS_ERROR("Could not load EGL library!");
            return false;
        }

        s_EGL.getConfigAttrib = (PFN_eglGetConfigAttrib)Platform::GetLibraryProcAddress(s_EGL.handle, "eglGetConfigAttrib");
        s_EGL.getConfigs = (PFN_eglGetConfigs)Platform::GetLibraryProcAddress(s_EGL.handle, "eglGetConfigs");
        s_EGL.getDisplay = (PFN_eglGetDisplay)Platform::GetLibraryProcAddress(s_EGL.handle, "eglGetDisplay");
        s_EGL.getError = (PFN_eglGetError)Platform::GetLibraryProcAddress(s_EGL.handle, "eglGetError");
        s_EGL.initialize = (PFN_eglInitialize)Platform::GetLibraryProcAddress(s_EGL.handle, "eglInitialize");
        s_EGL.terminate = (PFN_eglTerminate)Platform::GetLibraryProcAddress(s_EGL.handle, "eglTerminate");
        s_EGL.bindAPI = (PFN_eglBindAPI)Platform::GetLibraryProcAddress(s_EGL.handle, "eglBindAPI");
        s_EGL.createContext = (PFN_eglCreateContext)Platform::GetLibraryProcAddress(s_EGL.handle, "eglCreateContext");
        s_EGL.destroySurface = (PFN_eglDestroySurface)Platform::GetLibraryProcAddress(s_EGL.handle, "eglDestroySurface");
        s_EGL.destroyContext = (PFN_eglDestroyContext)Platform::GetLibraryProcAddress(s_EGL.handle, "eglDestroyContext");
        s_EGL.createWindowSurface = (PFN_eglCreateWindowSurface)Platform::GetLibraryProcAddress(s_EGL.handle, "eglCreateWindowSurface");
        s_EGL.makeCurrent = (PFN_eglMakeCurrent)Platform::GetLibraryProcAddress(s_EGL.handle, "eglMakeCurrent");
        s_EGL.swapBuffers = (PFN_eglSwapBuffers)Platform::GetLibraryProcAddress(s_EGL.handle, "eglSwapBuffers");
        s_EGL.swapInterval = (PFN_eglSwapInterval)Platform::GetLibraryProcAddress(s_EGL.handle, "eglSwapInterval");
        s_EGL.queryString = (PFN_eglQueryString)Platform::GetLibraryProcAddress(s_EGL.handle, "eglQueryString");
        s_EGL.getProcAddress = (PFN_eglGetProcAddress)Platform::GetLibraryProcAddress(s_EGL.handle, "eglGetProcAddress");

        if (!s_EGL.getConfigAttrib
            || !s_EGL.getConfigs
            || !s_EGL.getDisplay
            || !s_EGL.getError
            || !s_EGL.initialize
            || !s_EGL.terminate
            || !s_EGL.bindAPI
            || !s_EGL.createContext
            || !s_EGL.destroySurface
            || !s_EGL.destroyContext
            || !s_EGL.createWindowSurface
            || !s_EGL.makeCurrent
            || !s_EGL.swapBuffers
            || !s_EGL.swapInterval
            || !s_EGL.queryString
            || !s_EGL.getProcAddress)
        {
            DAIS_ERROR("Could not load required EGL entry points!");
            terminate();
            return false;
        }

        const char* extensions = s_EGL.queryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
        if (extensions
            && s_EGL.getError() == EGL_SUCCESS)
        {
            s_EGL.EXT_ClientExtensions = true;
        }

        if (s_EGL.EXT_ClientExtensions)
        {
            s_EGL.EXT_PlatformBase = StringInExtensionString("EGL_EXT_platform_base", extensions);
            s_EGL.EXT_PlatformX11 = StringInExtensionString("EGL_EXT_platform_x11", extensions);
            s_EGL.EXT_PlatformWayland = StringInExtensionString("EGL_EXT_platform_wayland", extensions);
            s_EGL.ANGLE_PlatformAngle = StringInExtensionString("EGL_ANGLE_platform_angle", extensions);
            s_EGL.ANGLE_PlatformAngleOpenGL = StringInExtensionString("EGL_ANGLE_platform_angle_opengl", extensions);
            s_EGL.ANGLE_PlatformAngleD3D = StringInExtensionString("EGL_ANGLE_platform_angle_d3d", extensions);
            s_EGL.ANGLE_PlatformAngleVulkan = StringInExtensionString("EGL_ANGLE_platform_angle_vulkan", extensions);
            s_EGL.ANGLE_PlatformAngleMetal = StringInExtensionString("EGL_ANGLE_platform_angle_metal", extensions);
        }

        if (s_EGL.EXT_PlatformBase)
        {
            s_EGL.getPlarformDisplayEXT = (PFNEGLGETPLATFORMDISPLAYEXTPROC)s_EGL.getProcAddress("eglGetPlatformDisplayEXT");
            s_EGL.createPlatformWindowSurfaceEXT = (PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC)s_EGL.getProcAddress("eglCraetePlatformWindowSurfaceEXT");
        }

        EGLint* attribs = nullptr;
        s_EGL.platform = Platform::GetEglPlatform(&attribs);
        if (s_EGL.platform)
        {
            s_EGL.display = s_EGL.getPlarformDisplayEXT(s_EGL.platform, Platform::GetEglNativeDisplay(), attribs);
        }
        else
        {
            s_EGL.display = s_EGL.getDisplay(Platform::GetEglNativeDisplay());
        }

        free(attribs);

        if (s_EGL.display == EGL_NO_DISPLAY)
        {
            DAIS_ERROR("Failed to get EGL display: %s", GetErrorString(s_EGL.getError()));
            terminate();
            return false;
        }

        if (!s_EGL.initialize(s_EGL.display, &s_EGL.major, &s_EGL.minor))
        {
            DAIS_ERROR("Failed to initialize EGL: %s", GetErrorString(s_EGL.getError()));
            terminate();
            return false;
        }

        extensions = s_EGL.queryString(s_EGL.display, EGL_EXTENSIONS);

        s_EGL.KHR_CreateContext = StringInExtensionString("EGL_KHR_create_context", extensions);
        s_EGL.KHR_CreateContextNoError = StringInExtensionString("EGL_KHR_create_context_no_error", extensions);
        s_EGL.KHR_GlColorspace = StringInExtensionString("EGL_KHR_gl_colorspace", extensions);
        s_EGL.KHR_GetAllProcAddresses = StringInExtensionString("EGL_KHR_get_all_proc_addresses", extensions);
        s_EGL.KHR_ContextFlushControl = StringInExtensionString("EGL_KHR_context_flush_control", extensions);

        return true;
    }

    void EglContext::Terminate()
    {
        if (s_EGL.display)
        {
            s_EGL.terminate(s_EGL.display);
            s_EGL.display = EGL_NO_DISPLAY;
        }

        if (s_EGL.handle)
        {
            Platform::CloseLibrary(s_EGL.handle);
            s_EGL.handle = nullptr;
        }
    }


    bool EglContext::CreateContext(Window* window, const ContextConfig* contextConfig, const FramebufferConfig* framebufferConfig)
    {
        if (!s_EGL.display)
        {
            DAIS_ERROR("EGL API is not avaliable!");
            return false;
        }

        //cache the sharing context if exists
        EGLContext share = nullptr;
        if (contextConfig->share)
        {
            share = ((EglContext*)contextConfig->share)->m_Handle;
        }

        EGLConfig config;
        if (!GetClosestEGLConfig(contextConfig, framebufferConfig, &config))
        {
            DAIS_ERROR("Failed to find a suitable EGLConfig!");
            return false;
        }

        if (contextConfig->api == ContextAPI::OpenGLES)
        {
            if (!s_EGL.bindAPI(EGL_OPENGL_ES_API))
            {
                DAIS_ERROR("Failed to bind EGL API OpenGLES: %s", GetErrorString(s_EGL.getError()));
                return false;
            }
        }
        else
        {
            if (!s_EGL.bindAPI(EGL_OPENGL_API))
            {
                DAIS_ERROR("Failed to bind EGL API OpenGL: %s", GetErrorString(s_EGL.getError()));
                return false;
            }
        }

        std::vector<EGLint> attribs = {};

        if (s_EGL.KHR_CreateContext)
        {
            int32_t mask = 0;
            int32_t flags = 0;

            if (contextConfig->api == ContextAPI::OpenGL)
            {
                if (contextConfig->forward)
                {
                    flags |= EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR;
                }

                if (contextConfig->profile == ContextProfile::Core)
                {
                    mask |= EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR;
                }
                else if (contextConfig->profile == ContextProfile::Compatibility)
                {
                    mask |= EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR;
                }
            }

            if (contextConfig->debug)
            {
                flags |= EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR;
            }

            if (contextConfig->robustness != ContextRobustnessMode::None)
            {
                if (contextConfig->robustness == ContextRobustnessMode::NoResetNotification)
                {
                    attribs.push_back(EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_KHR);
                    attribs.push_back(EGL_NO_RESET_NOTIFICATION_KHR);
                }
                else if (contextConfig->robustness == ContextRobustnessMode::LoseContextOnReset)
                {
                    attribs.push_back(EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_KHR);
                    attribs.push_back(EGL_LOSE_CONTEXT_ON_RESET_KHR);
                }

                flags |= EGL_CONTEXT_OPENGL_ROBUST_ACCESS_BIT_KHR;
            }

            if (contextConfig->noerror)
            {
                if (s_EGL.KHR_CreateContextNoError)
                {
                    attribs.push_back(EGL_CONTEXT_OPENGL_NO_ERROR_KHR);
                    attribs.push_back(true);
                }
            }

            if (contextConfig->major != 1
                || contextConfig->minor != 0)
            {
                attribs.push_back(EGL_CONTEXT_MAJOR_VERSION_KHR);
                attribs.push_back(contextConfig->major);

                attribs.push_back(EGL_CONTEXT_MINOR_VERSION_KHR);
                attribs.push_back(contextConfig->minor);
            }

            if (mask)
            {
                attribs.push_back(EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR);
                attribs.push_back(mask);
            }

            if (flags)
            {
                attribs.push_back(EGL_CONTEXT_FLAGS_KHR);
                attribs.push_back(flags);
            }
        }
        else
        {
            if (contextConfig->api == ContextAPI::OpenGLES)
            {
                attribs.push_back(EGL_CONTEXT_CLIENT_VERSION);
                attribs.push_back(contextConfig->major);
            }
        }

        if (s_EGL.KHR_ContextFlushControl)
        {
            if (contextConfig->release == ContextReleaseBehavior::None)
            {
                attribs.push_back(EGL_CONTEXT_RELEASE_BEHAVIOR_KHR);
                attribs.push_back(EGL_CONTEXT_RELEASE_BEHAVIOR_NONE_KHR);
            }
            else if (contextConfig->release == ContextReleaseBehavior::Flush)
            {
                attribs.push_back(EGL_CONTEXT_RELEASE_BEHAVIOR_KHR);
                attribs.push_back(EGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_KHR);
            }
        }

        attribs.push_back(EGL_NONE);
        attribs.push_back(EGL_NONE);

        EglContext* eglContext = ((EglContext*)window->GetContext());
        eglContext->m_Handle = s_EGL.createContext(s_EGL.display, config, share, attribs.data());

        if (eglContext->m_Handle == EGL_NO_CONTEXT)
        {
            DAIS_ERROR("Failed to create EGL context: %s", GetErrorString(s_EGL.getError()));
            return false;
        }


        //setup attributes for surface creation
        attribs.clear();

        if (framebufferConfig->sRGB)
        {
            if (s_EGL.KHR_GlColorspace)
            {
                attribs.push_back(EGL_GL_COLORSPACE_KHR);
                attribs.push_back(EGL_GL_COLORSPACE_SRGB_KHR);
            }
        }

        attribs.push_back(EGL_NONE);
        attribs.push_back(EGL_NONE);

        EGLNativeWindowType native = Platform::GetEglNativeWindow(window);
        //HACK: ANGLE does not implement eglCratePlatformWindowSurfaceEXT despite reporting EGL_EXT_platform_base
        if (s_EGL.platform
            && s_EGL.platform != EGL_PLATFORM_ANGLE_ANGLE)
        {
            eglContext->m_Surface = s_EGL.createPlatformWindowSurfaceEXT(s_EGL.display, config, native, attribs.data());
        }
        else
        {
            eglContext->m_Surface = s_EGL.createWindowSurface(s_EGL.display, config, native, attribs.data());
        }

        if (eglContext->m_Surface == EGL_NO_SURFACE)
        {
            DAIS_ERROR("Failed to create EGL window surface: %s", GetErrorString(s_EGL.getError()));
            return false;
        }

        eglContext->m_Config = config;

        //load the appropriate client library
        if (!s_EGL.KHR_GetAllProcAddresses)
        {
            //LEARN C++: is this how you store const ref returned from the functions?
            std::vector<std::string> libNames;

            if (contextConfig->api == ContextAPI::OpenGLES)
            {
                if (contextConfig->major == 1)
                {
                    libNames = Platform::GetGLES1LibNames();
                }
                else
                {
                    libNames = Platform::GetGLES2LibNames();
                }
            }
            else
            {
                libNames = Platform::GetGLSLibNames();
            }

            for (const std::string& libName : libNames)
            {
                //HACK: match presence of lib prefix to increase chance of finding
                //a matching pair in the jungle that is Win32 EGL/GLES
                if (s_EGL.prefix != (libName.find("lib") == 0))
                {
                    continue;
                }

                eglContext->m_Client = Platform::OpenLibrary(libName);
                if (eglContext->m_Client)
                {
                    break;
                }
            }

            if (!eglContext->m_Client)
            {
                DAIS_ERROR("Failed to load EGL client library!");
                return false;
            }
        }

        return true;
    }


    void EglContext::EGLMakeContextCurrent(Window* window)
    {
        if (window)
        {
            EglContext* eglContext = (EglContext*)window->GetContext();

            if (!s_EGL.makeCurrent(s_EGL.display, eglContext->m_Surface, eglContext->m_Surface, eglContext->m_Handle))
            {
                DAIS_ERROR("Failed to make EGL context current: %s", GetErrorString(s_EGL.getError()));
                return;
            }
        }
        else
        {
            if (!s_EGL.makeCurrent(s_EGL.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
            {
                DAIS_ERROR("Failed to clear EGL context: %s", GetErrorString(s_EGL.getError()));
                return;
            }
        }

        Platform::s_ContextSlot->Set(window);
    }

    void EglContext::EGLSwapBuffers(Window* window)
    {
        Window* current = (Window*)Platform::s_ContextSlot->Get();
        if (window != current)
        {
            DAIS_ERROR("The EGL context must be current on the calling thread when swapping buffers!");
            return;
        }

        EglContext* eglContext = (EglContext*)window->GetContext();

        s_EGL.swapBuffers(s_EGL.display, eglContext->m_Surface);
    }

    void EglContext::EGLSwapInterval(int32_t interval)
    {
        s_EGL.swapInterval(s_EGL.display, interval);
    }

    bool EglContext::EGLExtensionSupported(const char* extension)
    {
        const char* extensions = EglContext::s_EGL.queryString(EglContext::s_EGL.display, EGL_EXTENSIONS);

        return extensions
            && StringInExtensionString(extension, extensions);
    }

    GLProc EglContext::EGLGetGLProcAddress(const char* procedureName)
    {
        Window* current = (Window*)Platform::s_ContextSlot->Get();

        EglContext* eglContext = (EglContext*)current->GetContext();

        if (eglContext->m_Client)
        {
            GLProc proc = (GLProc)Platform::GetLibraryProcAddress(eglContext->m_Client, procedureName);
            if (proc)
            {
                return proc;
            }
        }

        return s_EGL.getProcAddress(procedureName);
    }

    void EglContext::EGLDestroyContext(Window* window)
    {
        EglContext* eglContext = (EglContext*)window->GetContext();

        //NOTE: On X11, do not unload libGL.so.1 while the X11 display is still
        //open, as it will make XCloseDisplay segfault. add the following if
        //if (window->GetContext()->m_API != ContextAPI::OpenGL)
        {
            if (eglContext->m_Client)
            {
                Platform::CloseLibrary(eglContext->m_Client);
                eglContext->m_Client = nullptr;
            }
        }

        if (eglContext->m_Surface)
        {
            s_EGL.destroySurface(s_EGL.display, eglContext->m_Surface);
            eglContext->m_Surface = EGL_NO_SURFACE;
        }

        if (eglContext->m_Handle)
        {
            s_EGL.destroyContext(s_EGL.display, eglContext->m_Handle);
            eglContext->m_Handle = EGL_NO_CONTEXT;
        }
    }



    ///////////////////////////////////////// UTILS ///////////////////////////////////////////

    const char* EglContext::GetErrorString(EGLint error)
    {
        switch (error)
        {
            case EGL_SUCCESS: return "Success!";
            case EGL_NOT_INITIALIZED: return "EGL is not or could not be initialized!";
            case EGL_BAD_ACCESS: return "EGL cannot access a requested resource!";
            case EGL_BAD_ALLOC: return "EGL failed to allocate resources for the requested operation!";
            case EGL_BAD_ATTRIBUTE: return "An unrecognized attribute or attribute value was passed in the attribute list!";
            case EGL_BAD_CONTEXT: return "An EGLContext argument does not name a valid EGL rendering context!";
            case EGL_BAD_CONFIG: return "An EGLConfig argument does not name a valid EGL frame buffer configuration!";
            case EGL_BAD_CURRENT_SURFACE: return "The current suface of the calling thread is a window, pixel buffer or pixmap that is no longer valid!";
            case EGL_BAD_DISPLAY: return "An EGLDisplay argument does not name a valid EGL display connection!";
            case EGL_BAD_SURFACE: return "An EGLSurface argument does not name a valid surface configured for GL rendering!";
            case EGL_BAD_MATCH: return "Arguments are inconsistent!";
            case EGL_BAD_PARAMETER: return "One or more argument values are invalid!";
            case EGL_BAD_NATIVE_PIXMAP: return "A NativePixmapType argument does not refer to a valid native pixmap!";
            case EGL_BAD_NATIVE_WINDOW: return "A NativeWindowType argument does not refer to a valid native window!";
            case EGL_CONTEXT_LOST: return "The application must destroy all contexts and reinitialize!";
            default: return "Unknown EGL error!";
        }
    }

    bool EglContext::GetClosestEGLConfig(const ContextConfig* contextConfig, const FramebufferConfig* framebufferConfig, EGLConfig* result)
    {
        int nativeCount;

        s_EGL.getConfigs(s_EGL.display, nullptr, 0, &nativeCount);
        if (!nativeCount)
        {
            DAIS_ERROR("No EGLConfigs found!");
            return false;
        }

        std::vector<FramebufferConfig> usableConfigs = {};
        usableConfigs.reserve(nativeCount);

        std::vector<EGLConfig> nativeConfigs = {};
        nativeConfigs.reserve(nativeCount);

        s_EGL.getConfigs(s_EGL.display, nativeConfigs.data(), nativeCount, &nativeCount);

        for (int32_t i = 0; i < nativeCount; i++)
        {
            const EGLConfig n = nativeConfigs[i];

            int attribValue = 0;

            //only consider RGB(A) EGLConfigs
            s_EGL.getConfigAttrib(s_EGL.display, n, EGL_COLOR_BUFFER_TYPE, &attribValue);
            if (attribValue != EGL_RGB_BUFFER)
            {
                continue;
            }

            //only consider window EGLConfigs
            s_EGL.getConfigAttrib(s_EGL.display, n, EGL_SURFACE_TYPE, &attribValue);
            if (!(attribValue & EGL_WINDOW_BIT))
            {
                continue;
            }

            //TODO: handle X11 XVisualInfo?

            s_EGL.getConfigAttrib(s_EGL.display, n, EGL_RENDERABLE_TYPE, &attribValue);
            if (contextConfig->api == ContextAPI::OpenGLES)
            {
                if (contextConfig->major == 1)
                {
                    if (!(attribValue & EGL_OPENGL_ES_BIT))
                    {
                        continue;
                    }
                }
                else
                {
                    if (!(attribValue & EGL_OPENGL_ES2_BIT))
                    {
                        continue;
                    }
                }
            }
            else if (contextConfig->api == ContextAPI::OpenGL)
            {
                if (!(attribValue & EGL_OPENGL_BIT))
                {
                    continue;
                }
            }

            FramebufferConfig fbc = {};
            s_EGL.getConfigAttrib(s_EGL.display, n, EGL_RED_SIZE, &fbc.redBits);
            s_EGL.getConfigAttrib(s_EGL.display, n, EGL_GREEN_SIZE, &fbc.greenBits);
            s_EGL.getConfigAttrib(s_EGL.display, n, EGL_BLUE_SIZE, &fbc.blueBits);
            s_EGL.getConfigAttrib(s_EGL.display, n, EGL_ALPHA_SIZE, &fbc.alphaBits);
            s_EGL.getConfigAttrib(s_EGL.display, n, EGL_DEPTH_SIZE, &fbc.depthBits);
            s_EGL.getConfigAttrib(s_EGL.display, n, EGL_STENCIL_SIZE, &fbc.stencilBits);
            s_EGL.getConfigAttrib(s_EGL.display, n, EGL_SAMPLES, &fbc.samples);
            fbc.doubleBuffer = true;
            fbc.handle = (uintptr_t)n;

            usableConfigs.push_back(fbc);
        }

        if (!usableConfigs.size())
        {
            DAIS_ERROR("The driver does not appear to support OpenGL through EGL!");
            return false;
        }

        //pick the FramebufferConfig closest to the desired one
        const FramebufferConfig* closest = ChooseFramebufferConfig(framebufferConfig, usableConfigs);
        if (!closest)
        {
            DAIS_ERROR("Failed to find a suitable pixel format!");
            return false;
        }

        //we only care about the closest EGLConfig handle
        *result = (EGLConfig)closest->handle;

        return true;
    }
}
