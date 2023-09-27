#include "renderer/vulkan/os/vulkan_os.h"
#include "os/win32/os_win32.h"
#include "core/log.h"

B8 vulkanCreateSurfaceOS(
    VkInstance vkInstance, Window* pWindow, VkAllocationCallbacks* pVkAllocator, VkSurfaceKHR* out_pVkSurface
) {
    PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR =
        (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(vkInstance, "vkCreateWin32SurfaceKHR");

    if (vkCreateWin32SurfaceKHR == NULL) {
        logError("vkCreateWin32SurfaceKHR: Failed to load function");
        return false;
    }

    Win32Window* win32Window = (Win32Window*)pWindow->pOsState;

    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .hinstance = win32Window->instance,
        .hwnd = win32Window->handle,
    };

    if (vkCreateWin32SurfaceKHR(vkInstance, &surfaceCreateInfo, pVkAllocator, out_pVkSurface) != VK_SUCCESS) {
        logError("vkCreateWin32SurfaceKHR: Failed to create surface");
        return false;
    }

    return true;
}
