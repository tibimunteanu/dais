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
#define DAIS_ASSERT_MESSAGE(condition, ...) { if(!(condition)) { DAIS_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }

#define DAIS_ASSERT_RESOLVE(arg1, arg2, macro, ...) macro
#define DAIS_GET_ASSERT_MACRO(...) DAIS_EXPAND_VARGS(DAIS_ASSERT_RESOLVE(__VA_ARGS__, DAIS_ASSERT_MESSAGE, DAIS_ASSERT_NO_MESSAGE))

#define DAIS_ASSERT(...) DAIS_EXPAND_VARGS( DAIS_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#define DAIS_CORE_ASSERT(...) DAIS_EXPAND_VARGS( DAIS_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#else
#define DAIS_ASSERT(...)
#define DAIS_CORE_ASSERT(...)
#endif


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

struct Image
{
    int32_t width;
    int32_t height;
    uint8_t* pixels;
};

#include "engine/core/Utils.h"

#define DAIS_NO_API                          0
#define DAIS_OPENGL_API             0x00030001
#define DAIS_OPENGL_ES_API          0x00030002

#define DAIS_NO_ROBUSTNESS                   0
#define DAIS_NO_RESET_NOTIFICATION  0x00031001
#define DAIS_LOSE_CONTEXT_ON_RESET  0x00031002

#define DAIS_OPENGL_ANY_PROFILE              0
#define DAIS_OPENGL_CORE_PROFILE    0x00032001
#define DAIS_OPENGL_COMPAT_PROFILE  0x00032002

#define DAIS_ANY_RELEASE_BEHAVIOR            0
#define DAIS_RELEASE_BEHAVIOR_FLUSH 0x00035001
#define DAIS_RELEASE_BEHAVIOR_NONE  0x00035002

#define DAIS_NATIVE_CONTEXT_API     0x00036001
#define DAIS_EGL_CONTEXT_API        0x00036002
#define DAIS_OSMESA_CONTEXT_API     0x00036003

#define DAIS_POLL_PRESENCE          0
#define DAIS_POLL_AXES              1
#define DAIS_POLL_BUTTONS           2
#define DAIS_POLL_ALL               (DAIS_POLL_AXES | DAIS_POLL_BUTTONS)

#define DAIS_RELEASE                0
#define DAIS_PRESS                  1
#define DAIS_REPEAT                 2

#define DAIS_STICK                  3
#define DAIS_JOYSTICK_AXIS          1
#define DAIS_JOYSTICK_BUTTON        2
#define DAIS_JOYSTICK_HATBIT        3

#define DAIS_HAT_CENTERED           0
#define DAIS_HAT_UP                 1
#define DAIS_HAT_RIGHT              2
#define DAIS_HAT_DOWN               4
#define DAIS_HAT_LEFT               8
#define DAIS_HAT_RIGHT_UP           (DAIS_HAT_RIGHT | DAIS_HAT_UP)
#define DAIS_HAT_RIGHT_DOWN         (DAIS_HAT_RIGHT | DAIS_HAT_DOWN)
#define DAIS_HAT_LEFT_UP            (DAIS_HAT_LEFT  | DAIS_HAT_UP)
#define DAIS_HAT_LEFT_DOWN          (DAIS_HAT_LEFT  | DAIS_HAT_DOWN)

#define DAIS_KEY_UNKNOWN            -1

/* Printable keys */
#define DAIS_KEY_SPACE              32
#define DAIS_KEY_APOSTROPHE         39  /* ' */
#define DAIS_KEY_COMMA              44  /* , */
#define DAIS_KEY_MINUS              45  /* - */
#define DAIS_KEY_PERIOD             46  /* . */
#define DAIS_KEY_SLASH              47  /* / */
#define DAIS_KEY_0                  48
#define DAIS_KEY_1                  49
#define DAIS_KEY_2                  50
#define DAIS_KEY_3                  51
#define DAIS_KEY_4                  52
#define DAIS_KEY_5                  53
#define DAIS_KEY_6                  54
#define DAIS_KEY_7                  55
#define DAIS_KEY_8                  56
#define DAIS_KEY_9                  57
#define DAIS_KEY_SEMICOLON          59  /* ; */
#define DAIS_KEY_EQUAL              61  /* = */
#define DAIS_KEY_A                  65
#define DAIS_KEY_B                  66
#define DAIS_KEY_C                  67
#define DAIS_KEY_D                  68
#define DAIS_KEY_E                  69
#define DAIS_KEY_F                  70
#define DAIS_KEY_G                  71
#define DAIS_KEY_H                  72
#define DAIS_KEY_I                  73
#define DAIS_KEY_J                  74
#define DAIS_KEY_K                  75
#define DAIS_KEY_L                  76
#define DAIS_KEY_M                  77
#define DAIS_KEY_N                  78
#define DAIS_KEY_O                  79
#define DAIS_KEY_P                  80
#define DAIS_KEY_Q                  81
#define DAIS_KEY_R                  82
#define DAIS_KEY_S                  83
#define DAIS_KEY_T                  84
#define DAIS_KEY_U                  85
#define DAIS_KEY_V                  86
#define DAIS_KEY_W                  87
#define DAIS_KEY_X                  88
#define DAIS_KEY_Y                  89
#define DAIS_KEY_Z                  90
#define DAIS_KEY_LEFT_BRACKET       91  /* [ */
#define DAIS_KEY_BACKSLASH          92  /* \ */
#define DAIS_KEY_RIGHT_BRACKET      93  /* ] */
#define DAIS_KEY_GRAVE_ACCENT       96  /* ` */
#define DAIS_KEY_WORLD_1            161 /* non-US #1 */
#define DAIS_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define DAIS_KEY_ESCAPE             256
#define DAIS_KEY_ENTER              257
#define DAIS_KEY_TAB                258
#define DAIS_KEY_BACKSPACE          259
#define DAIS_KEY_INSERT             260
#define DAIS_KEY_DELETE             261
#define DAIS_KEY_RIGHT              262
#define DAIS_KEY_LEFT               263
#define DAIS_KEY_DOWN               264
#define DAIS_KEY_UP                 265
#define DAIS_KEY_PAGE_UP            266
#define DAIS_KEY_PAGE_DOWN          267
#define DAIS_KEY_HOME               268
#define DAIS_KEY_END                269
#define DAIS_KEY_CAPS_LOCK          280
#define DAIS_KEY_SCROLL_LOCK        281
#define DAIS_KEY_NUM_LOCK           282
#define DAIS_KEY_PRINT_SCREEN       283
#define DAIS_KEY_PAUSE              284
#define DAIS_KEY_F1                 290
#define DAIS_KEY_F2                 291
#define DAIS_KEY_F3                 292
#define DAIS_KEY_F4                 293
#define DAIS_KEY_F5                 294
#define DAIS_KEY_F6                 295
#define DAIS_KEY_F7                 296
#define DAIS_KEY_F8                 297
#define DAIS_KEY_F9                 298
#define DAIS_KEY_F10                299
#define DAIS_KEY_F11                300
#define DAIS_KEY_F12                301
#define DAIS_KEY_F13                302
#define DAIS_KEY_F14                303
#define DAIS_KEY_F15                304
#define DAIS_KEY_F16                305
#define DAIS_KEY_F17                306
#define DAIS_KEY_F18                307
#define DAIS_KEY_F19                308
#define DAIS_KEY_F20                309
#define DAIS_KEY_F21                310
#define DAIS_KEY_F22                311
#define DAIS_KEY_F23                312
#define DAIS_KEY_F24                313
#define DAIS_KEY_F25                314
#define DAIS_KEY_KP_0               320
#define DAIS_KEY_KP_1               321
#define DAIS_KEY_KP_2               322
#define DAIS_KEY_KP_3               323
#define DAIS_KEY_KP_4               324
#define DAIS_KEY_KP_5               325
#define DAIS_KEY_KP_6               326
#define DAIS_KEY_KP_7               327
#define DAIS_KEY_KP_8               328
#define DAIS_KEY_KP_9               329
#define DAIS_KEY_KP_DECIMAL         330
#define DAIS_KEY_KP_DIVIDE          331
#define DAIS_KEY_KP_MULTIPLY        332
#define DAIS_KEY_KP_SUBTRACT        333
#define DAIS_KEY_KP_ADD             334
#define DAIS_KEY_KP_ENTER           335
#define DAIS_KEY_KP_EQUAL           336
#define DAIS_KEY_LEFT_SHIFT         340
#define DAIS_KEY_LEFT_CONTROL       341
#define DAIS_KEY_LEFT_ALT           342
#define DAIS_KEY_LEFT_SUPER         343
#define DAIS_KEY_RIGHT_SHIFT        344
#define DAIS_KEY_RIGHT_CONTROL      345
#define DAIS_KEY_RIGHT_ALT          346
#define DAIS_KEY_RIGHT_SUPER        347
#define DAIS_KEY_MENU               348

#define DAIS_KEY_LAST               DAIS_KEY_MENU

#define DAIS_MOD_SHIFT           0x0001
#define DAIS_MOD_CONTROL         0x0002
#define DAIS_MOD_ALT             0x0004
#define DAIS_MOD_SUPER           0x0008
#define DAIS_MOD_CAPS_LOCK       0x0010
#define DAIS_MOD_NUM_LOCK        0x0020

#define DAIS_MOUSE_BUTTON_1         0
#define DAIS_MOUSE_BUTTON_2         1
#define DAIS_MOUSE_BUTTON_3         2
#define DAIS_MOUSE_BUTTON_4         3
#define DAIS_MOUSE_BUTTON_5         4
#define DAIS_MOUSE_BUTTON_6         5
#define DAIS_MOUSE_BUTTON_7         6
#define DAIS_MOUSE_BUTTON_8         7
#define DAIS_MOUSE_BUTTON_LAST      DAIS_MOUSE_BUTTON_8
#define DAIS_MOUSE_BUTTON_LEFT      DAIS_MOUSE_BUTTON_1
#define DAIS_MOUSE_BUTTON_RIGHT     DAIS_MOUSE_BUTTON_2
#define DAIS_MOUSE_BUTTON_MIDDLE    DAIS_MOUSE_BUTTON_3

#define DAIS_JOYSTICK_1             0
#define DAIS_JOYSTICK_2             1
#define DAIS_JOYSTICK_3             2
#define DAIS_JOYSTICK_4             3
#define DAIS_JOYSTICK_5             4
#define DAIS_JOYSTICK_6             5
#define DAIS_JOYSTICK_7             6
#define DAIS_JOYSTICK_8             7
#define DAIS_JOYSTICK_9             8
#define DAIS_JOYSTICK_10            9
#define DAIS_JOYSTICK_11            10
#define DAIS_JOYSTICK_12            11
#define DAIS_JOYSTICK_13            12
#define DAIS_JOYSTICK_14            13
#define DAIS_JOYSTICK_15            14
#define DAIS_JOYSTICK_16            15
#define DAIS_JOYSTICK_LAST          DAIS_JOYSTICK_16

#define DAIS_GAMEPAD_BUTTON_A               0
#define DAIS_GAMEPAD_BUTTON_B               1
#define DAIS_GAMEPAD_BUTTON_X               2
#define DAIS_GAMEPAD_BUTTON_Y               3
#define DAIS_GAMEPAD_BUTTON_LEFT_BUMPER     4
#define DAIS_GAMEPAD_BUTTON_RIGHT_BUMPER    5
#define DAIS_GAMEPAD_BUTTON_BACK            6
#define DAIS_GAMEPAD_BUTTON_START           7
#define DAIS_GAMEPAD_BUTTON_GUIDE           8
#define DAIS_GAMEPAD_BUTTON_LEFT_THUMB      9
#define DAIS_GAMEPAD_BUTTON_RIGHT_THUMB     10
#define DAIS_GAMEPAD_BUTTON_DPAD_UP         11
#define DAIS_GAMEPAD_BUTTON_DPAD_RIGHT      12
#define DAIS_GAMEPAD_BUTTON_DPAD_DOWN       13
#define DAIS_GAMEPAD_BUTTON_DPAD_LEFT       14
#define DAIS_GAMEPAD_BUTTON_LAST            DAIS_GAMEPAD_BUTTON_DPAD_LEFT

#define DAIS_GAMEPAD_BUTTON_CROSS       DAIS_GAMEPAD_BUTTON_A
#define DAIS_GAMEPAD_BUTTON_CIRCLE      DAIS_GAMEPAD_BUTTON_B
#define DAIS_GAMEPAD_BUTTON_SQUARE      DAIS_GAMEPAD_BUTTON_X
#define DAIS_GAMEPAD_BUTTON_TRIANGLE    DAIS_GAMEPAD_BUTTON_Y

#define DAIS_GAMEPAD_AXIS_LEFT_X        0
#define DAIS_GAMEPAD_AXIS_LEFT_Y        1
#define DAIS_GAMEPAD_AXIS_RIGHT_X       2
#define DAIS_GAMEPAD_AXIS_RIGHT_Y       3
#define DAIS_GAMEPAD_AXIS_LEFT_TRIGGER  4
#define DAIS_GAMEPAD_AXIS_RIGHT_TRIGGER 5
#define DAIS_GAMEPAD_AXIS_LAST          DAIS_GAMEPAD_AXIS_RIGHT_TRIGGER

// These are the standard cursor shapes that can be requested from the window system.
#define DAIS_ARROW_CURSOR           0x00036001  // The regular arrow cursor shape.
#define DAIS_IBEAM_CURSOR           0x00036002  // The text input I-beam cursor shape.
#define DAIS_CROSSHAIR_CURSOR       0x00036003  // The crosshair cursor shape.
#define DAIS_POINTING_HAND_CURSOR   0x00036004  // The pointing hand cursor shape.
#define DAIS_RESIZE_EW_CURSOR       0x00036005  // The horizontal resize/move arrow shape.  This is usually a horizontal double-headed arrow.
#define DAIS_RESIZE_NS_CURSOR       0x00036006  // The vertical resize/move shape.  This is usually a vertical double-headed  arrow.
#define DAIS_RESIZE_NWSE_CURSOR     0x00036007  // The top-left to bottom-right diagonal resize/move shape.  This is usually a diagonal double-headed arrow.
#define DAIS_RESIZE_NESW_CURSOR     0x00036008  // The top-right to bottom-left diagonal resize/move shape.  This is usually a diagonal double-headed arrow.
#define DAIS_RESIZE_ALL_CURSOR      0x00036009  // The omni-directional resize cursor/move shape.  This is usually either a combined horizontal and vertical double-headed arrow or a grabbing hand.
#define DAIS_NOT_ALLOWED_CURSOR     0x0003600A  // The operation-not-allowed shape.  This is usually a circle with a diagonal line through it.

#define DAIS_CURSOR                 0x00033001
#define DAIS_STICKY_KEYS            0x00033002
#define DAIS_STICKY_MOUSE_BUTTONS   0x00033003
#define DAIS_LOCK_KEY_MODS          0x00033004
#define DAIS_RAW_MOUSE_MOTION       0x00033005

#define DAIS_CURSOR_NORMAL          0x00034001
#define DAIS_CURSOR_HIDDEN          0x00034002
#define DAIS_CURSOR_DISABLED        0x00034003

//The regular arrow cursor shape.
#define DAIS_ARROW_CURSOR           0x00036001

//The text input I-beam cursor shape.
#define DAIS_IBEAM_CURSOR           0x00036002

//The crosshair cursor shape.
#define DAIS_CROSSHAIR_CURSOR       0x00036003

//The pointing hand cursor shape.
#define DAIS_POINTING_HAND_CURSOR   0x00036004

//The horizontal resize/move arrow shape.  This is usually a horizontal double-headed arrow.
#define DAIS_RESIZE_EW_CURSOR       0x00036005

//The vertical resize/move shape.  This is usually a vertical double-headed arrow.
#define DAIS_RESIZE_NS_CURSOR       0x00036006

//The top-left to bottom-right diagonal resize/move shape.  This is usually a diagonal double-headed arrow.
//NOTE: macos - This shape is provided by a private system API and may fail with ref DAIS_CURSOR_UNAVAILABLE in the future.
//NOTE: x11 - This shape is provided by a newer standard not supported by all cursor themes.
//NOTE: wayland - This shape is provided by a newer standard not supported by all cursor themes.
#define DAIS_RESIZE_NWSE_CURSOR     0x00036007

//The top-right to bottom-left diagonal resize/move shape.  This is usually a diagonal double-headed arrow.
//NOTE: macos - This shape is provided by a private system API and may fail with DAIS_CURSOR_UNAVAILABLE in the future.
//NOTE: x11 - This shape is provided by a newer standard not supported by all cursor themes.
//NOTE: wayland - This shape is provided by a newer standard not supported by all cursor themes.
#define DAIS_RESIZE_NESW_CURSOR     0x00036008

//The omni-directional resize cursor/move shape.  This is usually either a
//combined horizontal and vertical double-headed arrow or a grabbing hand.
#define DAIS_RESIZE_ALL_CURSOR      0x00036009

//The operation-not-allowed shape.  This is usually a circle with a diagonal line through it.
//NOTE: x11 - This shape is provided by a newer standard not supported by all cursor themes.
//NOTE: wayland - This shape is provided by a newer standard not supported by all cursor themes.
#define DAIS_NOT_ALLOWED_CURSOR     0x0003600A

#define DAIS_JOYSTICK_HAT_BUTTONS   0x00050001
