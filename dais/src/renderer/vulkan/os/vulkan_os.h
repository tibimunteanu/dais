#pragma once

#include "base/base.h"

#if defined(OS_WINDOWS)
#    define VK_USE_PLATFORM_WIN32_KHR
#    define VK_OS_EXTENSION_NAMES "VK_KHR_win32_surface"
#endif
#include "renderer/vulkan/vendor/volk.h"

#include "os/os.h"

B8 vulkanCreateSurfaceOS(
    VkInstance vkInstance, Window* pWindow, VkAllocationCallbacks* pVkAllocator, VkSurfaceKHR* out_pVkSurface
);
