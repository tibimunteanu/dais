#pragma once

#include "base_platform_detection.h"

#if defined(EXPORT)
    #define API __declspec(dllexport)
#else
    #define API __declspec(dllimport)
#endif

#define inl   __attribute__((always_inline)) inline
#define noinl __attribute__((noinline))
