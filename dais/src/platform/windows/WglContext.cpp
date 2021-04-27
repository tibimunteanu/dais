#include "platform/windows/WindowsPlatform.h"

namespace dais
{
    ////////////////////////////////////// STATIC INIT ////////////////////////////////////////

    WglContext::WglLib WglContext::s_WGL = {};



    ///////////////////////////////////// INTERNAL API ////////////////////////////////////////

    bool WglContext::InitWGL()
    {
        if (s_WGL.instance)
        {
            return true;
        }

        s_WGL.instance = LoadLibraryA("opengl32.dll");
        if (!s_WGL.instance)
        {
            DAIS_ERROR("Failed to load opengl32.dll!");
            return false;
        }

        //load common wgl function pointers from opengl32.dll
        s_WGL.createContext = (PFNWGLCREATECONTEXTPROC)GetProcAddress(s_WGL.instance, "wglCreateContext");
        s_WGL.deleteContext = (PFNWGLDELETECONTEXTPROC)GetProcAddress(s_WGL.instance, "wglDeleteContext");
        s_WGL.getProcAddress = (PFNWGLGETPROCADDRESSPROC)GetProcAddress(s_WGL.instance, "wglGetProcAddress");
        s_WGL.getCurrentDC = (PFNWGLGETCURRENTDCPROC)GetProcAddress(s_WGL.instance, "wglGetCurrentDC");
        s_WGL.getCurrentContext = (PFNWGLGETCURRENTCONTEXTPROC)GetProcAddress(s_WGL.instance, "wglGetCurrentContext");
        s_WGL.makeCurrent = (PFNWGLMAKECURRENTPROC)GetProcAddress(s_WGL.instance, "wglMakeCurrent");
        s_WGL.shareLists = (PFNWGLSHARELISTSPROC)GetProcAddress(s_WGL.instance, "wglShareLists");

        //get a dummy DC from the helper window
        HDC dummyDC = GetDC(WindowsPlatform::s_HelperWindowHandle);

        //choose a dummy pixel format
        PIXELFORMATDESCRIPTOR dummyPFD = {};
        dummyPFD.nSize = sizeof(dummyPFD);
        dummyPFD.nVersion = 1;
        dummyPFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        dummyPFD.iPixelType = PFD_TYPE_RGBA;
        dummyPFD.cColorBits = 24;

        int dummyPFDID = ChoosePixelFormat(dummyDC, &dummyPFD);
        if (dummyPFDID == 0)
        {
            DAIS_ERROR("Failed to choose pixel format for dummy context!");
            return false;
        }

        //set the dummy pixel format
        if (!SetPixelFormat(dummyDC, dummyPFDID, &dummyPFD))
        {
            DAIS_ERROR("Failed to set pixel format for dummy context!");
            return false;
        }

        //create dummy rendering context
        HGLRC dummyRC = s_WGL.createContext(dummyDC);
        if (!dummyRC)
        {
            DAIS_ERROR("Failed to create dummy rendering context!");
            return false;
        }

        HDC prevDC = s_WGL.getCurrentDC();
        HGLRC prevRC = s_WGL.getCurrentContext();

        //make the dummy context current
        if (!s_WGL.makeCurrent(dummyDC, dummyRC))
        {
            DAIS_ERROR("Failed to make dummy context current!");

            //restore previous context and delete dummy context
            s_WGL.makeCurrent(prevDC, prevRC);
            s_WGL.deleteContext(dummyRC);

            return false;
        }

        //load function pointers for common extensions
        s_WGL.getExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)s_WGL.getProcAddress("wglGetExtensionsStringEXT");
        s_WGL.getExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)s_WGL.getProcAddress("wglGetExtensionsStringARB");
        s_WGL.createContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)s_WGL.getProcAddress("wglCreateContextAttribsARB");
        s_WGL.swapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)s_WGL.getProcAddress("wglSwapIntervalEXT");
        s_WGL.getPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)s_WGL.getProcAddress("wglGetPixelFormatAttribivARB");

        //cache some common extension support
        s_WGL.ARB_Multisample = WglContext::ExtensionSupportedWGL("WGL_ARB_multisample");
        s_WGL.ARB_FramebufferSRGB = WglContext::ExtensionSupportedWGL("WGL_ARB_framebuffer_sRGB");
        s_WGL.EXT_FramebufferSRGB = WglContext::ExtensionSupportedWGL("WGL_EXT_framebuffer_sRGB");
        s_WGL.ARB_CreateContext = WglContext::ExtensionSupportedWGL("WGL_ARB_create_context");
        s_WGL.ARB_CreateContextProfile = WglContext::ExtensionSupportedWGL("WGL_ARB_create_context_pprofile");
        s_WGL.EXT_CreateContextES2Profile = WglContext::ExtensionSupportedWGL("WGL_EXT_create_context_es2_profile");
        s_WGL.ARB_CreateContextRobustness = WglContext::ExtensionSupportedWGL("WGL_ARB_create_context_robustness");
        s_WGL.ARB_CreateContextNoError = WglContext::ExtensionSupportedWGL("WGL_ARB_create_context_no_error");
        s_WGL.EXT_SwapControl = WglContext::ExtensionSupportedWGL("WGL_EXT_swap_control");
        s_WGL.EXT_Colorspace = WglContext::ExtensionSupportedWGL("WGL_EXT_colorspace");
        s_WGL.ARB_PixelFormat = WglContext::ExtensionSupportedWGL("WGL_ARB_pixel_format");
        s_WGL.ARB_ContextFlushControl = WglContext::ExtensionSupportedWGL("WGL_ARB_context_flush_control");

        //restore previous context and delete dummy context
        s_WGL.makeCurrent(prevDC, prevRC);
        s_WGL.deleteContext(dummyRC);

        return true;
    }

    void WglContext::TerminateWGL()
    {
        if (s_WGL.instance)
        {
            FreeLibrary(s_WGL.instance);
            s_WGL = {};
        }
    }


    bool WglContext::CreateContextWGL(WindowsWindow* window, const ContextConfig* contextConfig, const FramebufferConfig* framebufferConfig)
    {
        //check required extensions needed for creating the context
        if (contextConfig->client == DAIS_OPENGL_API)
        {
            if (contextConfig->forward)
            {
                if (!s_WGL.ARB_CreateContext)
                {
                    DAIS_ERROR("A forward compatible OpenGL context requested but WGL_ARB_create_context is unavailable!");
                    return false;
                }
            }

            if (contextConfig->profile)
            {
                if (!s_WGL.ARB_CreateContextProfile)
                {
                    DAIS_ERROR("OpenGL context profile requested but WGL_ARB_create_context_profile is unavailable!");
                    return false;
                }
            }
        }
        else
        {
            if (!s_WGL.ARB_CreateContext
                || !s_WGL.ARB_CreateContextProfile
                || !s_WGL.EXT_CreateContextES2Profile)
            {
                DAIS_ERROR("OpenGL ES requested but WGL_ARB_create_context_es2_profile is unavailable!");
                return false;
            }
        }

        //cache the sharing context if exists
        HGLRC share = NULL;
        if (contextConfig->share)
        {
            share = ((WglContext*)contextConfig->share->GetContext())->m_Handle;
        }

        //set the context dc handle
        HDC dc = GetDC(window->m_Handle);
        if (!dc)
        {
            DAIS_ERROR("Failed to retrieve DC for window!");
            return false;
        }

        ((WglContext*)window->GetContext())->m_DC = dc;

        //get the pixelFormat closest to the desired ContextConfig and FramebufferConfig
        int32_t pixelFormat = ChoosePixelFormatWGL(window, contextConfig, framebufferConfig);
        if (!pixelFormat)
        {
            return false;
        }

        //SetPixelFormat requires old PIXELFORMATDESCRIPTOR struct
        //we can create it from the pixelFormat handle
        PIXELFORMATDESCRIPTOR pfd;
        if (!DescribePixelFormat(dc, pixelFormat, sizeof(pfd), &pfd))
        {
            DAIS_ERROR("Failed to retrieve PFD for selected pixel format!");
            return false;
        }

        //set the chosen pixel format
        if (!SetPixelFormat(dc, pixelFormat, &pfd))
        {
            DAIS_ERROR("Failed to set selected pixel format!");
            return false;
        }

        if (s_WGL.ARB_CreateContext)
        {
            //create the context through the "modern" extension with attribs
            int32_t mask = 0;
            int32_t flags = 0;
            std::vector<int32_t> attribs = {};

            if (contextConfig->client == DAIS_OPENGL_API)
            {
                if (contextConfig->forward)
                {
                    flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
                }

                if (contextConfig->profile == DAIS_OPENGL_CORE_PROFILE)
                {
                    mask |= WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
                }
                else if (contextConfig->profile == DAIS_OPENGL_COMPAT_PROFILE)
                {
                    mask |= WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
                }
            }
            else
            {
                mask |= WGL_CONTEXT_ES2_PROFILE_BIT_EXT;
            }

            if (contextConfig->debug)
            {
                flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
            }

            if (contextConfig->robustness)
            {
                if (s_WGL.ARB_CreateContextRobustness)
                {
                    if (contextConfig->robustness == DAIS_NO_RESET_NOTIFICATION)
                    {
                        attribs.push_back(WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB);
                        attribs.push_back(WGL_NO_RESET_NOTIFICATION_ARB);
                    }
                    else if (contextConfig->robustness == DAIS_LOSE_CONTEXT_ON_RESET)
                    {
                        attribs.push_back(WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB);
                        attribs.push_back(WGL_LOSE_CONTEXT_ON_RESET_ARB);
                    }

                    flags |= WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB;
                }
            }

            if (contextConfig->release)
            {
                if (s_WGL.ARB_ContextFlushControl)
                {
                    if (contextConfig->release == DAIS_RELEASE_BEHAVIOR_NONE)
                    {
                        attribs.push_back(WGL_CONTEXT_RELEASE_BEHAVIOR_ARB);
                        attribs.push_back(WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB);
                    }
                    else if (contextConfig->release == DAIS_RELEASE_BEHAVIOR_FLUSH)
                    {
                        attribs.push_back(WGL_CONTEXT_RELEASE_BEHAVIOR_ARB);
                        attribs.push_back(WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB);
                    }
                }
            }

            if (contextConfig->noerror)
            {
                if (s_WGL.ARB_CreateContextNoError)
                {
                    attribs.push_back(WGL_CONTEXT_OPENGL_NO_ERROR_ARB);
                    attribs.push_back(true);
                }
            }

            //NOTE: only request an explicitly versioned context when necessary,
            //as explicitly requesting version 1.0 does not always return the highest
            //version supported by the driver
            if (contextConfig->major != 1
                || contextConfig->minor != 0)
            {
                attribs.push_back(WGL_CONTEXT_MAJOR_VERSION_ARB);
                attribs.push_back(contextConfig->major);

                attribs.push_back(WGL_CONTEXT_MINOR_VERSION_ARB);
                attribs.push_back(contextConfig->minor);
            }

            if (flags)
            {
                attribs.push_back(WGL_CONTEXT_FLAGS_ARB);
                attribs.push_back(flags);
            }

            if (mask)
            {
                attribs.push_back(WGL_CONTEXT_PROFILE_MASK_ARB);
                attribs.push_back(mask);
            }

            attribs.push_back(0);
            attribs.push_back(0);

            HGLRC contextHandle = s_WGL.createContextAttribsARB(dc, share, attribs.data());
            if (!contextHandle)
            {
                const DWORD error = GetLastError();
                if (error == (0xc0070000 | ERROR_INVALID_VERSION_ARB))
                {
                    if (contextConfig->client == DAIS_OPENGL_API)
                    {
                        DAIS_ERROR("Driver does not support OpenGL version %i.%i", contextConfig->major, contextConfig->minor);
                    }
                    else
                    {
                        DAIS_ERROR("Driver does not support OpenGL ES version %i.%i", contextConfig->major, contextConfig->minor);
                    }
                }
                else if (error == (0xc0070000 | ERROR_INVALID_PROFILE_ARB))
                {
                    DAIS_ERROR("Driver does not support the requested OpenGL profile");
                }
                else if (error == (0xc0070000 | ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB))
                {
                    DAIS_ERROR("The share context is not compatible with the requested context!");
                }
                else
                {
                    if (contextConfig->client == DAIS_OPENGL_API)
                    {
                        DAIS_ERROR("Failed to create OpenGL context!");
                    }
                    else
                    {
                        DAIS_ERROR("Failed to create OpenGL ES context!");
                    }
                }

                return false;
            }

            ((WglContext*)window->m_Context)->m_Handle = contextHandle;
        }
        else
        {
            //create the context the old way
            HGLRC contextHandle = s_WGL.createContext(dc);
            if (!contextHandle)
            {
                DAIS_ERROR("Failed to create OpenGL context!");
                return false;
            }

            ((WglContext*)window->m_Context)->m_Handle = contextHandle;

            if (share)
            {
                if (!s_WGL.shareLists(share, contextHandle))
                {
                    DAIS_ERROR("Failed to enable sharing with specified OpenGL context!");
                    return false;
                }
            }
        }

        //supply the platform agnostic context with function pointers
        //TODO: use virtual functions instead
        window->m_Context->m_MakeCurrent = MakeContextCurrentWGL;
        window->m_Context->m_SwapBuffers = SwapBuffersWGL;
        window->m_Context->m_SwapInterval = SwapIntervalWGL;
        window->m_Context->m_ExtensionSupported = ExtensionSupportedWGL;
        window->m_Context->m_GetProcAddress = GetProcAddressWGL;
        window->m_Context->m_Destroy = DestroyContextWGL;

        return true;
    }

    int32_t WglContext::FindPixelFormatAttribValue(const int32_t* attribs, int32_t attribCount, const int32_t* values, int32_t attrib)
    {
        for (int32_t i = 0; i < attribCount; i++)
        {
            if (attribs[i] == attrib)
            {
                return values[i];
            }
        }

        DAIS_ERROR("Unknown pixel format attribute requested!");
        return 0;
    }

    int32_t WglContext::ChoosePixelFormatWGL(Window* window, const ContextConfig* contextConfig, const FramebufferConfig* framebufferConfig)
    {
        int32_t nativeCount;
        int32_t usableCount = 0;
        int32_t attribCount = 0;
        int32_t attribs[40]; // we know we don't use more than 40 attribs
        int32_t values[40];

        WglContext* context = (WglContext*)window->GetContext();

        if (s_WGL.ARB_PixelFormat)
        {
            //get native pixel format count through "modern" extension
            const int32_t attrib = WGL_NUMBER_PIXEL_FORMATS_ARB;

            if (!s_WGL.getPixelFormatAttribivARB(context->m_DC, 1, 0, 1, &attrib, &nativeCount))
            {
                DAIS_ERROR("OpenGL GetPixelFormatAttribivARB failed. Could not retrieve pixel format count!");
                return 0;
            }

            //setup an array of relevant attribs
            attribs[attribCount++] = WGL_SUPPORT_OPENGL_ARB;
            attribs[attribCount++] = WGL_DRAW_TO_WINDOW_ARB;
            attribs[attribCount++] = WGL_PIXEL_TYPE_ARB;
            attribs[attribCount++] = WGL_ACCELERATION_ARB;
            attribs[attribCount++] = WGL_RED_BITS_ARB;
            attribs[attribCount++] = WGL_RED_SHIFT_ARB;
            attribs[attribCount++] = WGL_GREEN_BITS_ARB;
            attribs[attribCount++] = WGL_GREEN_SHIFT_ARB;
            attribs[attribCount++] = WGL_BLUE_BITS_ARB;
            attribs[attribCount++] = WGL_BLUE_SHIFT_ARB;
            attribs[attribCount++] = WGL_ALPHA_BITS_ARB;
            attribs[attribCount++] = WGL_ALPHA_SHIFT_ARB;
            attribs[attribCount++] = WGL_DEPTH_BITS_ARB;
            attribs[attribCount++] = WGL_STENCIL_BITS_ARB;
            attribs[attribCount++] = WGL_ACCUM_BITS_ARB;
            attribs[attribCount++] = WGL_ACCUM_RED_BITS_ARB;
            attribs[attribCount++] = WGL_ACCUM_GREEN_BITS_ARB;
            attribs[attribCount++] = WGL_ACCUM_BLUE_BITS_ARB;
            attribs[attribCount++] = WGL_ACCUM_ALPHA_BITS_ARB;
            attribs[attribCount++] = WGL_AUX_BUFFERS_ARB;
            attribs[attribCount++] = WGL_STEREO_ARB;
            attribs[attribCount++] = WGL_DOUBLE_BUFFER_ARB;

            if (s_WGL.ARB_Multisample)
            {
                attribs[attribCount++] = WGL_SAMPLES_ARB;
            }

            if (contextConfig->client == DAIS_OPENGL_API)
            {
                if (s_WGL.ARB_FramebufferSRGB
                    || s_WGL.EXT_FramebufferSRGB)
                {
                    attribs[attribCount++] = WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB;
                }
            }
            else
            {
                if (s_WGL.EXT_Colorspace)
                {
                    attribs[attribCount++] = WGL_COLORSPACE_EXT;
                }
            }
        }
        else
        {
            //get native pixel format count the old way
            nativeCount = DescribePixelFormat(context->m_DC, 1, sizeof(PIXELFORMATDESCRIPTOR), NULL);
        }

        std::vector<FramebufferConfig> usableConfigs = {};
        usableConfigs.reserve(nativeCount);

        //iterate pixel formats, get the values for the attribs array 
        //and promote some of them as FramebufferConfigs
        int32_t pixelFormat;
        for (int32_t i = 0; i < nativeCount; i++)
        {
            pixelFormat = i + 1;

            FramebufferConfig fbc = {};

            if (s_WGL.ARB_PixelFormat)
            {
                //get pixel format attributes through "modern" extension
                if (!s_WGL.getPixelFormatAttribivARB(context->m_DC, pixelFormat, 0, attribCount, attribs, values))
                {
                    DAIS_ERROR("OpenGL GetPixelFormatAttribivARB failed. Could not retrieve pixel format attributes!");
                    return 0;
                }

                //first check hard constraints
                if (!FindPixelFormatAttribValue(attribs, attribCount, values, WGL_SUPPORT_OPENGL_ARB)
                    || !FindPixelFormatAttribValue(attribs, attribCount, values, WGL_DRAW_TO_WINDOW_ARB))
                {
                    continue;
                }

                if (FindPixelFormatAttribValue(attribs, attribCount, values, WGL_PIXEL_TYPE_ARB) != WGL_TYPE_RGBA_ARB)
                {
                    continue;
                }

                if (FindPixelFormatAttribValue(attribs, attribCount, values, WGL_ACCELERATION_ARB) == WGL_NO_ACCELERATION_ARB)
                {
                    continue;
                }

                //setup a FramebufferConfig from attrib values
                fbc.redBits = FindPixelFormatAttribValue(attribs, attribCount, values, WGL_RED_BITS_ARB);
                fbc.greenBits = FindPixelFormatAttribValue(attribs, attribCount, values, WGL_GREEN_BITS_ARB);
                fbc.blueBits = FindPixelFormatAttribValue(attribs, attribCount, values, WGL_BLUE_BITS_ARB);
                fbc.alphaBits = FindPixelFormatAttribValue(attribs, attribCount, values, WGL_ALPHA_BITS_ARB);
                fbc.depthBits = FindPixelFormatAttribValue(attribs, attribCount, values, WGL_DEPTH_BITS_ARB);
                fbc.stencilBits = FindPixelFormatAttribValue(attribs, attribCount, values, WGL_STENCIL_BITS_ARB);
                fbc.accumRedBits = FindPixelFormatAttribValue(attribs, attribCount, values, WGL_ACCUM_RED_BITS_ARB);
                fbc.accumGreenBits = FindPixelFormatAttribValue(attribs, attribCount, values, WGL_ACCUM_GREEN_BITS_ARB);
                fbc.accumBlueBits = FindPixelFormatAttribValue(attribs, attribCount, values, WGL_ACCUM_BLUE_BITS_ARB);
                fbc.accumAlphaBits = FindPixelFormatAttribValue(attribs, attribCount, values, WGL_ACCUM_ALPHA_BITS_ARB);
                fbc.auxBuffers = FindPixelFormatAttribValue(attribs, attribCount, values, WGL_AUX_BUFFERS_ARB);

                if (FindPixelFormatAttribValue(attribs, attribCount, values, WGL_STEREO_ARB))
                {
                    fbc.stereo = true;
                }

                if (FindPixelFormatAttribValue(attribs, attribCount, values, WGL_DOUBLE_BUFFER_ARB))
                {
                    fbc.doubleBuffer = true;
                }

                if (s_WGL.ARB_Multisample)
                {
                    fbc.samples = FindPixelFormatAttribValue(attribs, attribCount, values, WGL_SAMPLES_ARB);
                }

                if (contextConfig->client == DAIS_OPENGL_API)
                {
                    if (s_WGL.ARB_FramebufferSRGB
                        || s_WGL.EXT_FramebufferSRGB)
                    {
                        if (FindPixelFormatAttribValue(attribs, attribCount, values, WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB))
                        {
                            fbc.sRGB = true;
                        }
                    }
                }
                else
                {
                    if (s_WGL.EXT_Colorspace)
                    {
                        if (FindPixelFormatAttribValue(attribs, attribCount, values, WGL_COLORSPACE_EXT) == WGL_COLORSPACE_SRGB_EXT)
                        {
                            fbc.sRGB = true;
                        }
                    }
                }
            }
            else
            {
                //get pixel format attributes the old way
                PIXELFORMATDESCRIPTOR pfd;
                if (!DescribePixelFormat(context->m_DC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd))
                {
                    DAIS_ERROR("Failed to describe pixel format!");
                    return 0;
                }

                //first check hard constraints
                if (!(pfd.dwFlags & PFD_DRAW_TO_WINDOW)
                    || !(pfd.dwFlags & PFD_SUPPORT_OPENGL))
                {
                    continue;
                }

                if (!(pfd.dwFlags & PFD_GENERIC_ACCELERATED)
                    && (pfd.dwFlags & PFD_GENERIC_FORMAT))
                {
                    continue;
                }

                if (pfd.iPixelType != PFD_TYPE_RGBA)
                {
                    continue;
                }

                //setup a FramebufferConfig from the pixel format descriptor
                fbc.redBits = pfd.cRedBits;
                fbc.greenBits = pfd.cGreenBits;
                fbc.blueBits = pfd.cBlueBits;
                fbc.alphaBits = pfd.cAlphaBits;
                fbc.depthBits = pfd.cDepthBits;
                fbc.stencilBits = pfd.cStencilBits;
                fbc.accumRedBits = pfd.cAccumRedBits;
                fbc.accumGreenBits = pfd.cAccumGreenBits;
                fbc.accumBlueBits = pfd.cAccumBlueBits;
                fbc.accumAlphaBits = pfd.cAccumAlphaBits;
                fbc.auxBuffers = pfd.cAuxBuffers;

                if (pfd.dwFlags & PFD_STEREO)
                {
                    fbc.stereo = true;
                }

                if (pfd.dwFlags & PFD_DOUBLEBUFFER)
                {
                    fbc.doubleBuffer = true;
                }

            }

            fbc.handle = pixelFormat;
            usableConfigs.push_back(fbc);
        }

        if (!usableConfigs.size())
        {
            DAIS_ERROR("The driver does not appear to support OpenGL!");
            return 0;
        }

        //pick the FramebufferConfig closest to the desired one
        const FramebufferConfig* closest = ChooseFramebufferConfig(framebufferConfig, usableConfigs);
        if (!closest)
        {
            DAIS_ERROR("Failed to find a suitable pixel format!");
            return 0;
        }

        //we only care about the closest pixel format handle
        pixelFormat = (int32_t)closest->handle;

        return pixelFormat;
    }


    //TODO: make these virtual overrides
    void WglContext::MakeContextCurrentWGL(Window* window)
    {
        if (window)
        {
            WglContext* context = (WglContext*)window->GetContext();
            if (s_WGL.makeCurrent(context->m_DC, context->m_Handle))
            {
                Platform::s_ContextSlot->Set(window);
            }
            else
            {
                DAIS_ERROR("Failed to make context current!");
                Platform::s_ContextSlot->Set(nullptr);
            }
        }
        else
        {
            if (!s_WGL.makeCurrent(NULL, NULL))
            {
                DAIS_ERROR("Failed to clear current context!");
            }

            Platform::s_ContextSlot->Set(nullptr);
        }
    }

    void WglContext::SwapBuffersWGL(Window* window)
    {
        WglContext* context = (WglContext*)window->GetContext();

        if (!window->GetMonitor())
        {
            if (WindowsPlatform::IsWindowsVistaOrGreater())
            {
                //DWM composition is always enabled on Win8+
                BOOL enabled = WindowsPlatform::IsWindows8OrGreater();

                //HACK: use DwmFlush when desktop composition is enabled
                if (enabled
                    || (SUCCEEDED(WindowsPlatform::s_Libs.dwmapi.IsCompositionEnabled(&enabled)) && enabled))
                {
                    int32_t count = abs(context->m_Interval);
                    while (count--)
                    {
                        WindowsPlatform::s_Libs.dwmapi.Flush();
                    }
                }
            }
        }

        SwapBuffers(context->m_DC);
    }

    void WglContext::SwapIntervalWGL(int32_t interval)
    {
        Window* window = (Window*)WindowsPlatform::s_ContextSlot->Get();
        WglContext* context = (WglContext*)window->GetContext();

        context->m_Interval = interval;

        if (!window->GetMonitor())
        {
            if (WindowsPlatform::IsWindowsVistaOrGreater())
            {
                //DWM composition is always enabled on Win8+
                BOOL enabled = WindowsPlatform::IsWindows8OrGreater();

                //HACK: disable WGL swap interval when desktop composition is enabled to avoid interfering with DWM vsync
                if (enabled
                    || (SUCCEEDED(WindowsPlatform::s_Libs.dwmapi.IsCompositionEnabled(&enabled)) && enabled))
                {
                    interval = 0;
                }
            }
        }

        if (s_WGL.EXT_SwapControl)
        {
            s_WGL.swapIntervalEXT(interval);
        }
    }

    bool WglContext::ExtensionSupportedWGL(const char* extension)
    {
        const char* extensions = nullptr;

        if (s_WGL.getExtensionsStringARB)
        {
            extensions = s_WGL.getExtensionsStringARB(s_WGL.getCurrentDC());
        }
        else if (s_WGL.getExtensionsStringEXT)
        {
            extensions = s_WGL.getExtensionsStringEXT();
        }

        if (!extensions)
        {
            return false;
        }

        return Context::StringInExtensionString(extension, extensions);
    }

    GLProc WglContext::GetProcAddressWGL(const char* procedureName)
    {
        const GLProc proc = (GLProc)s_WGL.getProcAddress(procedureName);
        if (proc)
        {
            return proc;
        }

        return (GLProc)GetProcAddress(s_WGL.instance, procedureName);
    }

    void WglContext::DestroyContextWGL(Window* window)
    {
        WglContext* context = (WglContext*)window->GetContext();

        if (context->m_Handle)
        {
            s_WGL.deleteContext(context->m_Handle);
            context->m_Handle = NULL;
        }
    }
}
