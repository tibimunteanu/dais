#pragma once

#include "base/base.h"

#if defined(OS_WINDOWS)
    #define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "renderer/vulkan/vendor/volk.h"

typedef struct VulkanDeviceSwapchainSupport {
    VkSurfaceCapabilitiesKHR capabilities;
    U32 formatCount;
    VkSurfaceFormatKHR* pFormats;
    U32 presentModeCount;
    VkPresentModeKHR* pPresentModes;
} VulkanDeviceSwapchainSupport;
