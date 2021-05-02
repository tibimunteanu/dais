#pragma once

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <climits>
#include <cstdint>
#include <cfloat>
#include <string>
#include <vector>
#include <algorithm>
#include <malloc.h>

#define DAIS_PUBLIC_API 
#define DAIS_PLATFORM_API 
#define DAIS_INTERNAL_API 
#define DAIS_EVENT_INPUT_API 
#define DAIS_UTILS 

//HACK: __VA_ARGS__ expansion to get past MSVC "BUG"
#define DAIS_EXPAND_VARGS(x) x

//////////////////////////////////////// LOG ///////////////////////////////////////////
#ifdef DAIS_DEBUG
#define DAIS_ENABLE_LOG
#endif

#ifdef DAIS_ENABLE_LOG
#define DAIS_TRACE(...)	printf("TRACE: "); printf(__VA_ARGS__); printf("\n")
#define DAIS_INFO(...) printf("INFO: "); printf(__VA_ARGS__); printf("\n")
#define DAIS_WARN(...) printf("WARN: "); printf(__VA_ARGS__); printf("\n")
#define DAIS_ERROR(...) printf("ERROR: "); printf(__VA_ARGS__); printf("\n")
#define DAIS_FATAL(...) printf("FATAL: "); printf(__VA_ARGS__); printf("\n")
#else
#define DAIS_TRACE(...)
#define DAIS_INFO(...)
#define DAIS_WARN(...)
#define DAIS_ERROR(...)
#define DAIS_FATAL(...)
#endif

/////////////////////////////////////// ASSERT /////////////////////////////////////////
#ifdef DAIS_DEBUG
#define DAIS_ENABLE_ASSERTS
#endif

#ifdef DAIS_ENABLE_ASSERTS
#define DAIS_ASSERT_NO_MESSAGE(condition) { if(!(condition)) { DAIS_ERROR("Assertion Failed"); __debugbreak(); } }
#define DAIS_ASSERT_MESSAGE(condition, ...) { if(!(condition)) { DAIS_ERROR("Assertion Failed: %s", __VA_ARGS__); __debugbreak(); } }

#define DAIS_ASSERT_RESOLVE(arg1, arg2, macro, ...) macro
#define DAIS_GET_ASSERT_MACRO(...) DAIS_EXPAND_VARGS(DAIS_ASSERT_RESOLVE(__VA_ARGS__, DAIS_ASSERT_MESSAGE, DAIS_ASSERT_NO_MESSAGE))

#define DAIS_ASSERT(...) DAIS_EXPAND_VARGS( DAIS_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#define DAIS_CORE_ASSERT(...) DAIS_EXPAND_VARGS( DAIS_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#else
#define DAIS_ASSERT(...)
#define DAIS_CORE_ASSERT(...)
#endif

#define FLAG_OPERATORS(type, member) \
constexpr enum type operator |(const enum type a, const enum type b) { return (enum type)(static_cast<uint32_t>(a) | static_cast<uint32_t>(b)); } \
constexpr enum type operator &(const enum type a, const enum type b) { return (enum type)(static_cast<uint32_t>(a) & static_cast<uint32_t>(b)); } \
constexpr enum type operator ~(const enum type a) { return (enum type)(~static_cast<uint32_t>(a)); } \

///////////////////////////////// PLATFORM DETECTION ///////////////////////////////////
#ifdef _WIN32
// Windows x64/x86
#ifdef _WIN64
// Windows x64
#define DAIS_PLATFORM_WINDOWS
#else
    // Windows x86
#error "x86 Builds are not supported!"
#endif

#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>
// TARGET_OS_MAC exists on all the platforms so we must check all of them
// (in this order) to ensure that we're running on MAC and not some other Apple platform
#if TARGET_IPHONE_SIMULATOR == 1
#error "IOS simulator is not supported!"
#elif TARGET_OS_IPHONE == 1
#define DAIS_PLATFORM_IOS
#error "IOS is not supported!"
#elif TARGET_OS_MAC == 1
#define DAIS_PLATFORM_MAC
#error "MacOS is not supported!"
#else
#error "Unknown Apple platform!"
#endif
 // We also have to check __ANDROID__ before __linux__ since android is
 // based on the linux kernel it has __linux__ defined
#elif defined(__ANDROID__)
#define DAIS_PLATFORM_ANDROID
#error "Android is not supported!"
#elif defined(__linux__)
#define DAIS_PLATFORM_LINUX
#error "Linux is not supported!"
#else
    // Unknown compiler/platform
#error "Unknown platform!"
#endif 
// End of platform detection

//It is customary to use APIENTRY for OpenGL function pointer declarations on all platforms.
//Additionally, the Window OpenGL header needs APIENTRY.
#if !defined(APIENTRY)
#if defined(_WIN32)
#define APIENTRY __stdcall
#else
#define APIENTRY
#endif
#endif

//Some Windows OpenGL headers need this.
#if !defined(WINGDIAPI) && defined(_WIN32)
#define WINGDIAPI __declspec(dllimport)
#endif

//Some Windows GLU headers need this.
#if !defined(CALLBACK) && defined(_WIN32)
#define CALLBACK __stdcall
#endif

typedef void (*GLProc)(void);

#define GL_VERSION 0x1f02
#define GL_NONE 0
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_UNSIGNED_BYTE 0x1401
#define GL_EXTENSIONS 0x1f03
#define GL_NUM_EXTENSIONS 0x821d
#define GL_CONTEXT_FLAGS 0x821e
#define GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT 0x00000001
#define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
#define GL_CONTEXT_PROFILE_MASK 0x9126
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#define GL_CONTEXT_CORE_PROFILE_BIT 0x00000001
#define GL_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
#define GL_LOSE_CONTEXT_ON_RESET_ARB 0x8252
#define GL_NO_RESET_NOTIFICATION_ARB 0x8261
#define GL_CONTEXT_RELEASE_BEHAVIOR 0x82fb
#define GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH 0x82fc
#define GL_CONTEXT_FLAG_NO_ERROR_BIT_KHR 0x00000008

typedef int GLint;
typedef unsigned int GLuint;
typedef unsigned int GLenum;
typedef unsigned int GLbitfield;
typedef unsigned char GLubyte;

//function pointer typedefs from glcorearb.h
typedef void (APIENTRY* PFNGLCLEARPROC)(GLbitfield mask);
typedef const GLubyte* (APIENTRY* PFNGLGETSTRINGPROC)(GLenum name);
typedef void (APIENTRY* PFNGLGETINTEGERVPROC)(GLenum pname, GLint* data);
typedef const GLubyte* (APIENTRY* PFNGLGETSTRINGIPROC)(GLenum name, GLuint index);


#if defined(_WIN32)
 #define EGLAPIENTRY __stdcall
#else
 #define EGLAPIENTRY
#endif

#define EGL_SUCCESS 0x3000
#define EGL_NOT_INITIALIZED 0x3001
#define EGL_BAD_ACCESS 0x3002
#define EGL_BAD_ALLOC 0x3003
#define EGL_BAD_ATTRIBUTE 0x3004
#define EGL_BAD_CONFIG 0x3005
#define EGL_BAD_CONTEXT 0x3006
#define EGL_BAD_CURRENT_SURFACE 0x3007
#define EGL_BAD_DISPLAY 0x3008
#define EGL_BAD_MATCH 0x3009
#define EGL_BAD_NATIVE_PIXMAP 0x300a
#define EGL_BAD_NATIVE_WINDOW 0x300b
#define EGL_BAD_PARAMETER 0x300c
#define EGL_BAD_SURFACE 0x300d
#define EGL_CONTEXT_LOST 0x300e
#define EGL_COLOR_BUFFER_TYPE 0x303f
#define EGL_RGB_BUFFER 0x308e
#define EGL_SURFACE_TYPE 0x3033
#define EGL_WINDOW_BIT 0x0004
#define EGL_RENDERABLE_TYPE 0x3040
#define EGL_OPENGL_ES_BIT 0x0001
#define EGL_OPENGL_ES2_BIT 0x0004
#define EGL_OPENGL_BIT 0x0008
#define EGL_ALPHA_SIZE 0x3021
#define EGL_BLUE_SIZE 0x3022
#define EGL_GREEN_SIZE 0x3023
#define EGL_RED_SIZE 0x3024
#define EGL_DEPTH_SIZE 0x3025
#define EGL_STENCIL_SIZE 0x3026
#define EGL_SAMPLES 0x3031
#define EGL_OPENGL_ES_API 0x30a0
#define EGL_OPENGL_API 0x30a2
#define EGL_NONE 0x3038
#define EGL_EXTENSIONS 0x3055
#define EGL_CONTEXT_CLIENT_VERSION 0x3098
#define EGL_NATIVE_VISUAL_ID 0x302e
#define EGL_NO_SURFACE ((EGLSurface) 0)
#define EGL_NO_DISPLAY ((EGLDisplay) 0)
#define EGL_NO_CONTEXT ((EGLContext) 0)
#define EGL_DEFAULT_DISPLAY ((EGLNativeDisplayType) 0)

#define EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR 0x00000002
#define EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR 0x00000001
#define EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR 0x00000002
#define EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR 0x00000001
#define EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_KHR 0x31bd
#define EGL_NO_RESET_NOTIFICATION_KHR 0x31be
#define EGL_LOSE_CONTEXT_ON_RESET_KHR 0x31bf
#define EGL_CONTEXT_OPENGL_ROBUST_ACCESS_BIT_KHR 0x00000004
#define EGL_CONTEXT_MAJOR_VERSION_KHR 0x3098
#define EGL_CONTEXT_MINOR_VERSION_KHR 0x30fb
#define EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR 0x30fd
#define EGL_CONTEXT_FLAGS_KHR 0x30fc
#define EGL_CONTEXT_OPENGL_NO_ERROR_KHR 0x31b3
#define EGL_GL_COLORSPACE_KHR 0x309d
#define EGL_GL_COLORSPACE_SRGB_KHR 0x3089
#define EGL_CONTEXT_RELEASE_BEHAVIOR_KHR 0x2097
#define EGL_CONTEXT_RELEASE_BEHAVIOR_NONE_KHR 0
#define EGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_KHR 0x2098
#define EGL_PLATFORM_X11_EXT 0x31d5
#define EGL_PLATFORM_WAYLAND_EXT 0x31d8
#define EGL_PLATFORM_ANGLE_ANGLE 0x3202
#define EGL_PLATFORM_ANGLE_TYPE_ANGLE 0x3203
#define EGL_PLATFORM_ANGLE_TYPE_OPENGL_ANGLE 0x320d
#define EGL_PLATFORM_ANGLE_TYPE_OPENGLES_ANGLE 0x320e
#define EGL_PLATFORM_ANGLE_TYPE_D3D9_ANGLE 0x3207
#define EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE 0x3208
#define EGL_PLATFORM_ANGLE_TYPE_VULKAN_ANGLE 0x3450
#define EGL_PLATFORM_ANGLE_TYPE_METAL_ANGLE 0x3489
#define EGL_PLATFORM_ANGLE_NATIVE_PLATFORM_TYPE_ANGLE 0x348f

typedef int EGLint;
typedef unsigned int EGLBoolean;
typedef unsigned int EGLenum;
typedef void* EGLConfig;
typedef void* EGLContext;
typedef void* EGLDisplay;
typedef void* EGLSurface;

typedef void* EGLNativeDisplayType;
typedef void* EGLNativeWindowType;

// EGL function pointer typedefs
typedef EGLBoolean (EGLAPIENTRY * PFN_eglGetConfigAttrib)(EGLDisplay,EGLConfig,EGLint,EGLint*);
typedef EGLBoolean (EGLAPIENTRY * PFN_eglGetConfigs)(EGLDisplay,EGLConfig*,EGLint,EGLint*);
typedef EGLDisplay (EGLAPIENTRY * PFN_eglGetDisplay)(EGLNativeDisplayType);
typedef EGLint (EGLAPIENTRY * PFN_eglGetError)(void);
typedef EGLBoolean (EGLAPIENTRY * PFN_eglInitialize)(EGLDisplay,EGLint*,EGLint*);
typedef EGLBoolean (EGLAPIENTRY * PFN_eglTerminate)(EGLDisplay);
typedef EGLBoolean (EGLAPIENTRY * PFN_eglBindAPI)(EGLenum);
typedef EGLContext (EGLAPIENTRY * PFN_eglCreateContext)(EGLDisplay,EGLConfig,EGLContext,const EGLint*);
typedef EGLBoolean (EGLAPIENTRY * PFN_eglDestroySurface)(EGLDisplay,EGLSurface);
typedef EGLBoolean (EGLAPIENTRY * PFN_eglDestroyContext)(EGLDisplay,EGLContext);
typedef EGLSurface (EGLAPIENTRY * PFN_eglCreateWindowSurface)(EGLDisplay,EGLConfig,EGLNativeWindowType,const EGLint*);
typedef EGLBoolean (EGLAPIENTRY * PFN_eglMakeCurrent)(EGLDisplay,EGLSurface,EGLSurface,EGLContext);
typedef EGLBoolean (EGLAPIENTRY * PFN_eglSwapBuffers)(EGLDisplay,EGLSurface);
typedef EGLBoolean (EGLAPIENTRY * PFN_eglSwapInterval)(EGLDisplay,EGLint);
typedef const char* (EGLAPIENTRY * PFN_eglQueryString)(EGLDisplay,EGLint);
typedef GLProc (EGLAPIENTRY * PFN_eglGetProcAddress)(const char*);

typedef EGLDisplay (EGLAPIENTRY * PFNEGLGETPLATFORMDISPLAYEXTPROC)(EGLenum,void*,const EGLint*);
typedef EGLSurface (EGLAPIENTRY * PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC)(EGLDisplay,EGLConfig,void*,const EGLint*);


struct Image
{
    int32_t width;
    int32_t height;
    uint8_t* pixels;
};

#include "engine/core/Utils.h"

namespace dais
{
    //enums
    enum class ContextAPI
    {
        None = 0,
        OpenGL = 1,
        OpenGLES = 2
    };

    enum class ContextType
    {
        Native = 0,
        EGL = 1
    };

    enum class ContextRobustnessMode
    {
        None = 0,
        NoResetNotification = 1,
        LoseContextOnReset = 2
    };

    enum class ContextReleaseBehavior
    {
        Any = 0,
        Flush = 1,
        None = 2
    };

    enum class ContextProfile
    {
        Any = 0,
        Core = 1,
        Compatibility = 2
    };

    enum class AnglePlatformType
    {
        None = 0,

        OpenGL = 1,
        OpenGLES = 2,
        D3D9 = 3,
        D3D11 = 4,
        Vulkan = 5,
    };

    enum class InputMode
    {
        Cursor = 0,
        StickyKeys = 1,
        StickyMouseButtons = 2,
        LockKeyMods = 3,
        RawMouseMotion = 4
    };

    enum class CursorMode
    {
        Normal = 0,
        Hidden = 1,
        Disabled = 2
    };

    enum class CursorShape
    {
        //The regular arrow cursor shape.
        Arrow = 0,

        //The text input I-beam cursor shape.
        IBeam = 1,

        //The crosshair cursor shape.
        Crosshair = 2,

        //The pointing hand cursor shape.
        PointingHand = 3,

        //The horizontal resize/move arrow shape.  This is usually a horizontal double-headed arrow.
        ResizeEW = 4,

        //The vertical resize/move shape.  This is usually a vertical double-headed arrow.
        ResizeNS = 5,

        //The top-left to bottom-right diagonal resize/move shape.  This is usually a diagonal double-headed arrow.
        //NOTE: macos - This shape is provided by a private system API and may fail with ref DAIS_CURSOR_UNAVAILABLE in the future.
        //NOTE: x11 - This shape is provided by a newer standard not supported by all cursor themes.
        //NOTE: wayland - This shape is provided by a newer standard not supported by all cursor themes.
        ResizeNWSE = 6,

        //The top-right to bottom-left diagonal resize/move shape.  This is usually a diagonal double-headed arrow.
        //NOTE: macos - This shape is provided by a private system API and may fail with DAIS_CURSOR_UNAVAILABLE in the future.
        //NOTE: x11 - This shape is provided by a newer standard not supported by all cursor themes.
        //NOTE: wayland - This shape is provided by a newer standard not supported by all cursor themes.
        ResizeNESW = 7,

        //The omni-directional resize cursor/move shape.  This is usually either a
        //combined horizontal and vertical double-headed arrow or a grabbing hand.
        ResizeAll = 8,

        //The operation-not-allowed shape.  This is usually a circle with a diagonal line through it.
        //NOTE: x11 - This shape is provided by a newer standard not supported by all cursor themes.
        //NOTE: wayland - This shape is provided by a newer standard not supported by all cursor themes.
        NotAllowed = 9
    };

    enum class JoystickButtonType
    {
        None = 0,

        Axis = 1,
        Button = 2,
        HatBit = 3
    };

    enum class KeyState
    {
        Release = 0,
        Press = 1,
        Repeat = 2,
        Stick = 3
    };

    enum class HatState
    {
        Centered = 0,
        Up = 1 << 0,
        Right = 1 << 1,
        Down = 1 << 2,
        Left = 1 << 3,

        RightUp = (Right | Up),
        RightDown = (Right | Down),
        LeftUp = (Left | Up),
        LeftDown = (Left | Down)
    };
    FLAG_OPERATORS(HatState)

    enum class Key
    {
        Unknown = -1,

        /* Printable keys */
        Space = 32,
        Apostrophe = 39,
        Comma = 44,
        Minus = 45,
        Period = 46,
        Slash = 47,
        NumRow0 = 48,
        NumRow1 = 49,
        NumRow2 = 50,
        NumRow3 = 51,
        NumRow4 = 52,
        NumRow5 = 53,
        NumRow6 = 54,
        NumRow7 = 55,
        NumRow8 = 56,
        NumRow9 = 57,
        Semicolon = 59,
        Equal = 61,
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,
        LeftBracket = 91,
        Backslash = 92,
        RightBracket = 93,
        GraveAccent = 96,
        World1 = 161,
        World2 = 162,

        /* Function keys */
        Escape = 256,
        Enter = 257,
        Tab = 258,
        Backspace = 259,
        Insert = 260,
        Delete = 261,
        Right = 262,
        Left = 263,
        Down = 264,
        Up = 265,
        PageUp = 266,
        PageDown = 267,
        Home = 268,
        End = 269,
        CapsLock = 280,
        ScrollLock = 281,
        NumLock = 282,
        PrintScreen = 283,
        Pause = 284,
        F1 = 290,
        F2 = 291,
        F3 = 292,
        F4 = 293,
        F5 = 294,
        F6 = 295,
        F7 = 296,
        F8 = 297,
        F9 = 298,
        F10 = 299,
        F11 = 300,
        F12 = 301,
        F13 = 302,
        F14 = 303,
        F15 = 304,
        F16 = 305,
        F17 = 306,
        F18 = 307,
        F19 = 308,
        F20 = 309,
        F21 = 310,
        F22 = 311,
        F23 = 312,
        F24 = 313,
        F25 = 314,
        KeyPad0 = 320,
        KeyPad1 = 321,
        KeyPad2 = 322,
        KeyPad3 = 323,
        KeyPad4 = 324,
        KeyPad5 = 325,
        KeyPad6 = 326,
        KeyPad7 = 327,
        KeyPad8 = 328,
        KeyPad9 = 329,
        KeyPadDecimal = 330,
        KeyPadDivide = 331,
        KeyPadMultiply = 332,
        KeyPadSubtract = 333,
        KeyPadAdd = 334,
        KeyPadEnter = 335,
        KeyPadEqual = 336,
        LeftShift = 340,
        LeftControl = 341,
        LeftAlt = 342,
        LeftSuper = 343,
        RightShift = 344,
        RightControl = 345,
        RightAlt = 346,
        RightSuper = 347,
        Menu = 348,

        Count
    };

    enum class KeyMods
    {
        None = 0,

        Shift = 1 << 0,
        Control = 1 << 1,
        Alt = 1 << 2,
        Super = 1 << 3,
        CapsLock = 1 << 4,
        NumLock = 1 << 5
    };
    FLAG_OPERATORS(KeyMods)

    enum class MouseButton
    {
        Button1 = 0,
        Button2 = 1,
        Button3 = 2,
        Button4 = 3,
        Button5 = 4,
        Button6 = 5,
        Button7 = 6,
        Button8 = 7,

        Count,

        Left = Button1,
        Right = Button2,
        Middle = Button3
    };

    enum class GamepadButton
    {
        A = 0,
        B = 1,
        X = 2,
        Y = 3,
        LeftBumper = 4,
        RightBumber = 5,
        Back = 6,
        Start = 7,
        Guide = 8,
        LeftThumb = 9,
        RightThumb = 10,
        DPadUp = 11,
        DPadRight = 12,
        DPadDown = 13,
        DPadLeft = 14,

        Count,

        Cross = A,
        Circle = B,
        Square = X,
        Triangle = Y
    };

    enum class GamepadAxis
    {
        LeftX = 0,
        LeftY = 1,
        RightX = 2,
        RightY = 3,
        LeftTrigger = 4,
        RightTrigger = 5,

        Count
    };
}

