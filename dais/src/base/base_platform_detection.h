#pragma once

// BRIEF: detect current operating system
#define OS_WINDOWS       0
#define OS_LINUX         0
#define OS_ANDROID       0
#define OS_IOS           0
#define OS_IOS_SIMULATOR 0
#define OS_MAC           0

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#    undef OS_WINDOWS
#    define OS_WINDOWS 1
#elif defined(__linux__) || defined(__gnu_linux__)
#    undef OS_LINUX
#    define OS_LINUX 1
#    if defined(__ANDROID__)
#        undef OS_ANDROID
#        define OS_ANDROID 1
#    endif
#elif defined(__APPLE__) && defined(__MACH__)
#    include <TargetConditionals.h>
#    if TARGET_IPHONE_SIMULATOR
#        undef OS_IOS
#        undef OS_IOS_SIMULATOR
#        define OS_IOS           1
#        define OS_IOS_SIMULATOR 1
#    elif TARGET_OS_IPHONE
#        undef OS_IOS
#        define OS_IOS 1
#    elif TARGET_OS_MAC
#        undef OS_MAC
#        define OS_MAC 1
#    else
#        error missing Apple operating system detection
#    endif
#else
#    error missing operating system detection
#endif

// BRIEF: detect current architecture
#define ARCH_X64   0
#define ARCH_X86   0
#define ARCH_ARM   0
#define ARCH_ARM64 0

#if defined(__amd64__) || defined(_M_AMD64)
#    undef ARCH_X64
#    define ARCH_X64 1
#elif defined(__i386__) || defined(_M_I86)
#    undef ARCH_X86
#    define ARCH_X86 1
#elif defined(__arm__) || defined(_M_ARM)
#    undef ARCH_ARM
#    define ARCH_ARM 1
#elif defined(__aarch64__)
#    undef ARCH_ARM64
#    define ARCH_ARM64 1
#else
#    error missing architecture detection
#endif

// BRIEF: detect current compiler
#define COMP_CL    0
#define COMP_CLANG 0
#define COMP_GCC   0

#if defined(__clang__) || defined(__llvm__)
#    undef COMP_CLANG
#    define COMP_CLANG 1
#elif defined(_MSC_VER) && !defined(__clang__)
#    undef COMP_CL
#    define COMP_CL 1
#elif defined(__GNUC__)
#    undef COMP_GCC
#    define COMP_GCC 1
#else
#    error missing compiler detection
#endif

// BRIEF: provide enums and strings
typedef enum OperatingSystem {
    OPERATING_SYSTEM_UNKNOWN,
    OPERATING_SYSTEM_WINDOWS,
    OPERATING_SYSTEM_LINUX,
    OPERATING_SYSTEM_ANDROID,
    OPERATING_SYSTEM_IOS,
    OPERATING_SYSTEM_IOS_SIMULATOR,
    OPERATING_SYSTEM_MAC,
    OPERATING_SYSTEM_COUNT
} OperatingSystem;

typedef enum Architecture {
    ARCHITECTURE_UNKNOWN,
    ARCHITECTURE_X64,
    ARCHITECTURE_X86,
    ARCHITECTURE_ARM,
    ARCHITECTURE_ARM64,
    ARCHITECTURE_COUNT
} Architecture;

typedef enum Compiler {
    COMPILER_UNKNOWN,
    COMPILER_CLANG,
    COMPILER_CL,
    COMPILER_GCC,
    COMPILER_COUNT
} Compiler;

#if defined(OS_WINDOWS)
#    define CURRENT_OPERATING_SYSTEM OPERATING_SYSTEM_WINDOWS;
#elif defined(OS_LINUX)
#    define CURRENT_OPERATING_SYSTEM OPERATING_SYSTEM_LINUX;
#elif defined(OS_ANDROID)
#    define CURRENT_OPERATING_SYSTEM OPERATING_SYSTEM_ANDROID;
#elif defined(OS_IOS)
#    define CURRENT_OPERATING_SYSTEM OPERATING_SYSTEM_IOS;
#elif defined(OS_IOS_SIMULATOR)
#    define CURRENT_OPERATING_SYSTEM OPERATING_SYSTEM_IOS_SIMULATOR;
#elif defined(OS_MAC)
#    define CURRENT_OPERATING_SYSTEM OPERATING_SYSTEM_MAC;
#else
#    define CURRENT_OPERATING_SYSTEM OPERATING_SYSTEM_UNKNOWN;
#endif

#if defined(ARCH_X64)
#    define CURRENT_ARCHITECTURE ARCHITECTURE_X64;
#elif defined(ARCH_X86)
#    define CURRENT_ARCHITECTURE ARCHITECTURE_X86;
#elif defined(ARCH_ARM)
#    define CURRENT_ARCHITECTURE ARCHITECTURE_ARM;
#elif defined(ARCH_ARM64)
#    define CURRENT_ARCHITECTURE ARCHITECTURE_ARM64;
#else
#    define CURRENT_ARCHITECTURE ARCHITECTURE_UNKNOWN;
#endif

#if defined(COMP_CLANG)
#    define CURRENT_COMPILER COMPILER_CLANG;
#elif defined(COMP_CL)
#    define CURRENT_COMPILER COMPILER_CL;
#elif defined(COMP_GCC)
#    define CURRENT_COMPILER COMPILER_GCC;
#else
#    define CURRENT_COMPILER COMPILER_UNKNOWN;
#endif

#if defined(OS_WINDOWS)
#    define CURRENT_OPERATING_SYSTEM_STR "windows";
#elif defined(OS_LINUX)
#    define CURRENT_OPERATING_SYSTEM_STR "linux";
#elif defined(OS_ANDROID)
#    define CURRENT_OPERATING_SYSTEM_STR "android";
#elif defined(OS_IOS)
#    define CURRENT_OPERATING_SYSTEM_STR "ios";
#elif defined(OS_IOS_SIMULATOR)
#    define CURRENT_OPERATING_SYSTEM_STR "ios simulator";
#elif defined(OS_MAC)
#    define CURRENT_OPERATING_SYSTEM_STR "mac";
#else
#    define CURRENT_OPERATING_SYSTEM_STR "unknown";
#endif

#if defined(ARCH_X64)
#    define CURRENT_ARCHITECTURE_STR "x64";
#elif defined(ARCH_X86)
#    define CURRENT_ARCHITECTURE_STR "x86";
#elif defined(ARCH_ARM)
#    define CURRENT_ARCHITECTURE_STR "arm";
#elif defined(ARCH_ARM64)
#    define CURRENT_ARCHITECTURE_STR "arm64";
#else
#    define CURRENT_ARCHITECTURE_STR "unknown";
#endif

#if defined(COMP_CLANG)
#    define CURRENT_COMPILER_STR "clang";
#elif defined(COMP_CL)
#    define CURRENT_COMPILER_STR "cl";
#elif defined(COMP_GCC)
#    define CURRENT_COMPILER_STR "gcc";
#else
#    define CURRENT_COMPILER_STR "unknown";
#endif
