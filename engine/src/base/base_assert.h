#pragma once

#include "base_types.h"
#include "base_keywords.h"
#include "base_platform_detection.h"

// NOTE: disable assertions by commenting out the below line.
#define ASSERTIONS_ENABLED

#ifdef ASSERTIONS_ENABLED
#    if defined(COMP_CL)
#        include <intrin.h>
#        define debugBreak() __debugbreak()
#    else
#        define debugBreak() __builtin_trap()
#    endif

API void _logReportAssertionFailure(CStringLit expression, CStringLit message, CStringLit file, I32 line);

#    define assert(expr)                                                   \
        {                                                                  \
            if (expr) {                                                    \
            } else {                                                       \
                _logReportAssertionFailure(#expr, "", __FILE__, __LINE__); \
                debugBreak();                                              \
            }                                                              \
        }

#    define assertMsg(expr, message)                                            \
        {                                                                       \
            if (expr) {                                                         \
            } else {                                                            \
                _logReportAssertionFailure(#expr, message, __FILE__, __LINE__); \
                debugBreak();                                                   \
            }                                                                   \
        }
#else
#    define assert(expr)
#    define assertMsg(expr, message)
#endif

#if defined(COMP_CLANG) || defined(COMP_GCC)
#    define staticAssert _Static_assert
#else
#    define staticAssert static_assert
#endif
