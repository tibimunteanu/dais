#include "base/base.h"

#if defined(OS_WINDOWS)
#    include "platform/win32/win32_platform_types.h"
#    include "renderer/vulkan/vulkan_types.h"
#    include "core/log.h"

CStringLit vulkanPlatformGetSurfaceExtensionName(void) {
    return "VK_KHR_win32_surface";
}

B8 vulkanPlatformCreateSurface(
    VkInstance vkInstance,
    Platform* pPlatform,
    Window* pWindow,
    VkAllocationCallbacks* pVkAllocator,
    VkSurfaceKHR* out_pVkSurface
) {
    PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR =
        (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(vkInstance, "vkCreateWin32SurfaceKHR");

    if (vkCreateWin32SurfaceKHR == NULL) {
        logError("vkCreateWin32SurfaceKHR: Failed to load function");
        return false;
    }

    Win32Platform* win32Platform = pPlatform->pInternal;
    Win32Window* win32Window = pWindow->pInternal;

    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .hinstance = win32Platform->instance,
        .hwnd = win32Window->handle,
    };

    if (vkCreateWin32SurfaceKHR(vkInstance, &surfaceCreateInfo, pVkAllocator, out_pVkSurface) != VK_SUCCESS) {
        logError("vkCreateWin32SurfaceKHR: Failed to create surface");
        return false;
    }

    return true;
}
#endif
