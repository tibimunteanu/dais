#pragma once

#include "renderer/vulkan/os/vulkan_os.h"

typedef struct VulkanDeviceSwapchainSupport {
    VkSurfaceCapabilitiesKHR capabilities;
    U32 formatCount;
    VkSurfaceFormatKHR* pFormats;
    U32 presentModeCount;
    VkPresentModeKHR* pPresentModes;
} VulkanDeviceSwapchainSupport;