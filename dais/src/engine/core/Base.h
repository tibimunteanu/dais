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
#include <malloc.h>


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
	/* Windows x64/x86 */
	#ifdef _WIN64
		/* Windows x64  */
		#define DAIS_PLATFORM_WINDOWS
	#else
		/* Windows x86 */
		#error "x86 Builds are not supported!"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	/* TARGET_OS_MAC exists on all the platforms
	 * so we must check all of them (in this order)
	 * to ensure that we're running on MAC
	 * and not some other Apple platform */
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
/* We also have to check __ANDROID__ before __linux__
 * since android is based on the linux kernel
 * it has __linux__ defined */
#elif defined(__ANDROID__)
	#define DAIS_PLATFORM_ANDROID
	#error "Android is not supported!"
#elif defined(__linux__)
	#define DAIS_PLATFORM_LINUX
	#error "Linux is not supported!"
#else
	/* Unknown compiler/platform */
	#error "Unknown platform!"
#endif 
// End of platform detection

