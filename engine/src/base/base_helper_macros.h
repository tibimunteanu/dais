#pragma once

#include "base_types.h"

#include <string.h>
#include <stdlib.h>

// BRIEF: bypass macro quirks
#define STMNT(S) \
    do {         \
        s        \
    } while (0)

#define STRINGIFY_(S) #S
#define STRINGIFY(S)  STRINGIFY_(S)
#define GLUE_(A, B)   A##B
#define GLUE(A, B)    GLUE_(A, B)

#define IDENTIFIER_FROM_LINE(name) GLUE(name, __LINE__)

#define _GET_NTH_ARG(_1, _2, _3, _4, _5, N, ...) N
#define COUNT_VARARGS(...)                       _GET_NTH_ARG("ignored", ##__VA_ARGS__, 4, 3, 2, 1, 0)

#define _GET_OVERRIDE_01(_1, _2, NAME, ...)           NAME
#define _GET_OVERRIDE_012(_1, _2, _3, NAME, ...)      NAME
#define _GET_OVERRIDE_0123(_1, _2, _3, _4, NAME, ...) NAME

#define GET_OVERRIDE_01(A, B, ...)         _GET_OVERRIDE_01("ignored", __VA_ARGS__, B, A)(__VA_ARGS__)
#define GET_OVERRIDE_012(A, B, C, ...)     _GET_OVERRIDE_012("ignored", __VA_ARGS__, C, B, A)(__VA_ARGS__)
#define GET_OVERRIDE_0123(A, B, C, D, ...) _GET_OVERRIDE_0123("ignored", __VA_ARGS__, D, C, B, A)(__VA_ARGS__)

// BRIEF: defer
#define DEFER_BLOCK(start, end)         for (I32 _i_ = ((start), 0); _i_ == 0; _i_ += 1, (end))
#define DEFER_BLOCK_CHECKED(start, end) for (I32 _i_ = 2 * !(start); (_i_ == 2 ? ((end), 0) : !_i_); _i_ += 1, (end))

// BRIEF: type traits
#define arrayCount(a)                    (sizeof(a) / sizeof(*(a)))
#define intFromPtr(p)                    (U64)(((U8*)p) - 0)
#define ptrFromInt(i)                    (void*)(((U8*)0) + i)
#define member(T, memberName)            (((T*)0)->memberName)
#define offsetOf(T, memberName)          intFromPtr(&member(T, memberName))
#define baseFromMember(T, memberName, p) (T*)((U8*)(p) - offsetOf(T, memberName))
#define swap(T, a, b)                    STMNT(T tmp = a; a = b; b = tmp;)

#define min(a, b)      (((a) < (b)) ? (a) : (b))
#define max(a, b)      (((a) > (b)) ? (a) : (b))
#define clamp(a, x, b) (((x) < (a)) ? (a) : ((b) < (x)) ? (b) : (x))
#define clampTop(a, b) min(a, b)
#define clampBot(a, b) max(a, b)

#define squared(x)    ((x) * (x))
#define isPowerOf2(x) ((x) != 0) && (((x) & ((x) - 1)) == 0)

#define roundUpToMultipleOf(x, m) ((x) + (m) - 1 - ((x) + (m) - 1) % (m))
#define alignUpPow2(x, p)         (((x) + (p) - 1) & ~((p) - 1))
#define alignDownPow2(x, p)       ((x) & ~((p) - 1))

#define bytes(n)     (n)
#define kilobytes(n) (n << 10)
#define megabytes(n) (n << 20)
#define gigabytes(n) (((U64)n) << 30)
#define terabytes(n) (((U64)n) << 40)

#define thousand(n) ((n) * 1000ull)
#define million(n)  ((n) * 1000000ull)
#define billion(n)  ((n) * 1000000000ull)

// BRIEF: Memory
#define memoryCopy memcpy
#define memoryMove memmove
#define memorySet  memset

#define memoryZero(ptr, size) memorySet((ptr), 0, (size))
#define memoryZeroStruct(ptr) memoryZero((ptr), sizeof(*(ptr)))
#define memoryZeroArray(arr)  memoryZero((arr), sizeof(arr))

#define memoryCopyStruct(dst, src)                \
    do {                                          \
        assert(sizeof(*(dst)) == sizeof(*(src))); \
        memoryCopy((dst), (src), sizeof(*(dst))); \
    } while (0)

#define memoryCopyArray(dst, src)              \
    do {                                       \
        assert(sizeof(dst) == sizeof(src));    \
        memoryCopy((dst), (src), sizeof(src)); \
    } while (0)
