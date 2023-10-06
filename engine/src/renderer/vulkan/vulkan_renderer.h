#pragma once

#include "renderer/vulkan/vulkan_types.h"
#include "platform/platform_types.h"
#include "core/arena.h"

API fn vulkanRendererInit(Arena* pArena, Platform* pPlatform, Window* pWindow);

API void vulkanRendererRelease(void);

CStringLit vulkanPlatformGetSurfaceExtensionName(void);

fn vulkanPlatformCreateSurface(
    VkInstance vkInstance,
    Platform* pPlatform,
    Window* pWindow,
    VkAllocationCallbacks* pVkAllocator,
    VkSurfaceKHR* out_pVkSurface
);
