#pragma once

#include "base_types.h"
#include "base_keywords.h"
#include "base_platform_detection.h"

// NOTE: disable assert and staticAssert by commenting out the below line.
#define ASSERTIONS_ENABLED

#if defined(ASSERTIONS_ENABLED)
    #if defined(COMP_CL)
        #include <intrin.h>
        #define debugBreak() __debugbreak()
    #else
        #define debugBreak() __builtin_trap()
    #endif

    #define assertNoMsg(expr)                                                                        \
        {                                                                                            \
            if (expr) {                                                                              \
            } else {                                                                                 \
                logOutput(LOG_LEVEL_FATAL, "ASSERT (%s) failed - %s:%d", #expr, __FILE__, __LINE__); \
                debugBreak();                                                                        \
            }                                                                                        \
        }

    #define assertMsg(expr, message, ...)                                                                              \
        {                                                                                                              \
            if (expr) {                                                                                                \
            } else {                                                                                                   \
                logOutput(LOG_LEVEL_FATAL, "ASSERT (%s) failed - %s:%d - error: " message, #expr, __FILE__, __LINE__); \
                debugBreak();                                                                                          \
            }                                                                                                          \
        }

    #define assert(...) GET_OVERRIDE_012(_, assertNoMsg, assertMsg, __VA_ARGS__)
#else
    #define assertNoMsg(expr)
    #define assertMsg(expr, message)
    #define assert(...)
#endif

#define staticAssert _Static_assert
