#pragma once

#include "renderer/vulkan/vulkan_types.h"
#include "platform/platform_types.h"

API B8 vulkanRendererInit(Platform* pPlatform, Window* pWindow);

API void vulkanRendererRelease(void);

CStringLit vulkanPlatformGetSurfaceExtensionName(void);

B8 vulkanPlatformCreateSurface(
    VkInstance vkInstance,
    Platform* pPlatform,
    Window* pWindow,
    VkAllocationCallbacks* pVkAllocator,
    VkSurfaceKHR* out_pVkSurface
);
