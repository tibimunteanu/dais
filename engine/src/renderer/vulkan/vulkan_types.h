#pragma once

#include "base/base.h"

#include "renderer/vulkan/vendor/volk.h"

typedef struct VulkanDeviceSwapchainSupport {
    VkSurfaceCapabilitiesKHR capabilities;
    U32 formatCount;
    VkSurfaceFormatKHR* pFormats;
    U32 presentModeCount;
    VkPresentModeKHR* pPresentModes;
} VulkanDeviceSwapchainSupport;
