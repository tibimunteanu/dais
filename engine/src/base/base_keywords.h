#pragma once

#include "base_platform_detection.h"

#define public
#define private  static
#define internal static
#define global   extern

#if defined(EXPORT)
    #if defined(COMP_CL)
        #define API __declspec(dllexport)
    #else
        #define API __attribute__((visibility("default")))
    #endif
#else
    #if defined(COMP_CL)
        #define API __declspec(dllimport)
    #else
        #define API
    #endif
#endif

#if defined(COMP_CLANG) || defined(COMP_GCC)
    #define inl   __attribute__((always_inline)) inline
    #define noinl __attribute__((noinline))
#elif defined(COMP_CL)
    #define inl   __forceinline
    #define noinl __declspec(noinline)
#else
    #define inl static inline
    #define noinl
#endif
