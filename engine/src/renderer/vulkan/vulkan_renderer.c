#include "renderer/vulkan/vulkan_renderer.h"
#include "core/log.h"

#include <string.h>

prv VkAllocationCallbacks* pVkAllocator = NULL;
prv VkInstance vkInstance = VK_NULL_HANDLE;
prv VkSurfaceKHR vkSurface = VK_NULL_HANDLE;
prv VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;

prv fn _getDeviceSwapchainSupport(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VulkanDeviceSwapchainSupport* out_pDeviceSwapchainSupport
) {
    // Surface capabilities
    VkResult result =
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &out_pDeviceSwapchainSupport->capabilities);

    if (result != VK_SUCCESS) {
        error("Failed to get physical device surface caps");
    }

    // Surface formats
    result =
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &out_pDeviceSwapchainSupport->formatCount, NULL);

    if (result != VK_SUCCESS && result != VK_INCOMPLETE) {
        error("Failed to get physical device surface formats");
    }

    if (out_pDeviceSwapchainSupport->formatCount != 0) {
        if (!out_pDeviceSwapchainSupport->pFormats) {
            out_pDeviceSwapchainSupport->pFormats =
                (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * out_pDeviceSwapchainSupport->formatCount);
        }
        result = vkGetPhysicalDeviceSurfaceFormatsKHR(
            physicalDevice, surface, &out_pDeviceSwapchainSupport->formatCount, out_pDeviceSwapchainSupport->pFormats
        );

        if (result != VK_SUCCESS && result != VK_INCOMPLETE) {
            error("Failed to get physical device surface formats");
        }
    }

    // Present modes
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice, surface, &out_pDeviceSwapchainSupport->presentModeCount, NULL
    );

    if (result != VK_SUCCESS && result != VK_INCOMPLETE) {
        error("Failed to get surface present modes");
    }

    if (out_pDeviceSwapchainSupport->presentModeCount != 0) {
        if (!out_pDeviceSwapchainSupport->pPresentModes) {
            out_pDeviceSwapchainSupport->pPresentModes =
                (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * out_pDeviceSwapchainSupport->presentModeCount);
        }
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(
            physicalDevice,
            surface,
            &out_pDeviceSwapchainSupport->presentModeCount,
            out_pDeviceSwapchainSupport->pPresentModes
        );

        if (result != VK_SUCCESS && result != VK_INCOMPLETE) {
            error("Failed to get surface present modes");
        }
    }

    ok();
}

prv U32 _getPhysicalDeviceScore(VkPhysicalDevice physicalDevice) {
    // Check for required features
    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

    if (!physicalDeviceFeatures.samplerAnisotropy) {
        return 0;
    }

    // Check for required queue families
    U32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);
    queueFamilyCount = min(queueFamilyCount, 32);
    VkQueueFamilyProperties queueFamilyProperties[32];
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties);

    I32 graphicsQueueFamilyIndex = -1;
    I32 presentQueueFamilyIndex = -1;
    I32 computeQueueFamilyIndex = -1;
    I32 transferQueueFamilyIndex = -1;

    // find a queue family that can do graphics and present to surface
    for (U32 i = 0; i < queueFamilyCount; i++) {
        VkQueueFlags flags = queueFamilyProperties[i].queueFlags;

        if (flags & VK_QUEUE_GRAPHICS_BIT) {
            VkBool32 canPresentToSurface = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, vkSurface, &canPresentToSurface);
            if (canPresentToSurface) {
                presentQueueFamilyIndex = i;
                graphicsQueueFamilyIndex = i;
            }
        }
    }

    U8 minTransferScore = MAX_U8;
    for (U32 i = 0; i < queueFamilyCount; i++) {
        VkQueueFlags flags = queueFamilyProperties[i].queueFlags;

        U8 transferScore = 0;

        if (flags & VK_QUEUE_GRAPHICS_BIT) {
            if (graphicsQueueFamilyIndex == -1) {
                graphicsQueueFamilyIndex = i;
            }
            if (graphicsQueueFamilyIndex == i) {
                transferScore++;
            }
        }

        if (presentQueueFamilyIndex == -1) {
            VkBool32 canPresentToSurface = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, vkSurface, &canPresentToSurface);
            if (canPresentToSurface) {
                presentQueueFamilyIndex = i;
            }
        }

        if (computeQueueFamilyIndex == -1 && (flags & VK_QUEUE_COMPUTE_BIT)) {
            computeQueueFamilyIndex = i;
            transferScore++;
        }

        if (transferScore < minTransferScore && (flags & VK_QUEUE_TRANSFER_BIT)) {
            transferQueueFamilyIndex = i;
            minTransferScore = transferScore;
        }
    }

    if (graphicsQueueFamilyIndex == -1 || presentQueueFamilyIndex == -1 || computeQueueFamilyIndex == -1 ||
        transferQueueFamilyIndex == -1) {
        return 0;
    }

    // Check for swapchain support
    VulkanDeviceSwapchainSupport vulkanDeviceSwapchainSupport = {0};
    if (catch (_getDeviceSwapchainSupport(physicalDevice, vkSurface, &vulkanDeviceSwapchainSupport))) {
        return 0;
    }
    if (vulkanDeviceSwapchainSupport.formatCount == 0 || vulkanDeviceSwapchainSupport.presentModeCount == 0) {
        if (vulkanDeviceSwapchainSupport.pFormats) {
            free(vulkanDeviceSwapchainSupport.pFormats);
        }
        if (vulkanDeviceSwapchainSupport.pPresentModes) {
            free(vulkanDeviceSwapchainSupport.pPresentModes);
        }
        return 0;
    }

    // Check for required extensions
    const CString deviceExtensionNames[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    if (arrayCount(deviceExtensionNames) > 0) {
        U32 availableExtensionsCount = 0;

        VkResult result = vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &availableExtensionsCount, NULL);

        if (result != VK_SUCCESS && result != VK_INCOMPLETE) {
            logError("vkEnumerateDeviceExtensionProperties: Failed to get device extension count");
            return 0;
        }

        if (availableExtensionsCount == 0) {
            return 0;
        }

        VkExtensionProperties* availableExtensions =
            (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * availableExtensionsCount);

        result =
            vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &availableExtensionsCount, availableExtensions);

        if (result != VK_SUCCESS && result != VK_INCOMPLETE) {
            logError("vkEnumerateDeviceExtensionProperties: Failed to enumerate device extensions");
            free(availableExtensions);
            return 0;
        }

        for (U32 i = 0; i < arrayCount(deviceExtensionNames); i++) {
            B8 found = false;

            for (U32 j = 0; j < availableExtensionsCount; j++) {
                if (strcmp(deviceExtensionNames[i], availableExtensions[j].extensionName) == 0) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                free(availableExtensions);
                return 0;
            }
        }

        free(availableExtensions);
    }

    // Compute score
    U32 score = 1;

    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

    B8 supportsDeviceLocalHostVisible = false;
    for (U32 i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++) {
        VkMemoryPropertyFlags flags = physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags;

        if (((flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0) &&
            ((flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)) {
            supportsDeviceLocalHostVisible = true;
            break;
        }
    }

    if (supportsDeviceLocalHostVisible) {
        score += 500;
    }

    if (physicalDeviceFeatures.geometryShader) {
        score += 100;
    }

    if (physicalDeviceFeatures.tessellationShader) {
        score += 50;
    }

    // Print device info
    logTrace(
        "%s gpu \"%s\" (score: %u)",
        physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_OTHER            ? "other"
        : physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? "integrated"
        : physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU   ? "discrete"
        : physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU    ? "virtual"
        : physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU            ? "cpu"
                                                                                        : "unknown",
        physicalDeviceProperties.deviceName,
        score
    );

    for (U32 j = 0; j < physicalDeviceMemoryProperties.memoryHeapCount; ++j) {
        VkMemoryHeap heap = physicalDeviceMemoryProperties.memoryHeaps[j];
        F32 heapSizeInGigabytes = (((F32)heap.size) / 1024.0f / 1024.0f / 1024.0f);

        if (heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
            logTrace("\tLocal GPU memory: %.2f GiB", heapSizeInGigabytes);
        } else {
            logTrace("\tShared System memory: %.2f GiB", heapSizeInGigabytes);
        }
    }

    return score;
}

//
pub fn vulkanRendererInit(Arena* pArena, Platform* pPlatform, Window* pWindow) {
    if (volkInitialize() != VK_SUCCESS) {
        error("Failed to initialize vulkan loader");
    }

    // Instance
    VkApplicationInfo applicationInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = "Dais Game",
        .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
        .pEngineName = "Dais Engine",
        .engineVersion = VK_MAKE_VERSION(0, 0, 1),
        .apiVersion = VK_API_VERSION_1_3,
    };

    const CString surfaceExtensionName = vulkanPlatformGetSurfaceExtensionName();
    const CString instanceExtensionNames[] = {VK_KHR_SURFACE_EXTENSION_NAME, surfaceExtensionName};
    const CString instanceLayers[] = {};

    VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = arrayCount(instanceLayers),
        .ppEnabledLayerNames = instanceLayers,
        .enabledExtensionCount = arrayCount(instanceExtensionNames),
        .ppEnabledExtensionNames = instanceExtensionNames,
    };

    if (vkCreateInstance(&instanceCreateInfo, pVkAllocator, &vkInstance) != VK_SUCCESS) {
        error("Failed to create vulkan instance");
    }

    volkLoadInstanceOnly(vkInstance);

    // Version
    U32 vkInstanceVersion;
    if (vkEnumerateInstanceVersion(&vkInstanceVersion) != VK_SUCCESS) {
        error("Failed to get vulkan instance version");
    }

    logTrace(
        "Vulkan instance version: %d.%d.%d",
        VK_VERSION_MAJOR(vkInstanceVersion),
        VK_VERSION_MINOR(vkInstanceVersion),
        VK_VERSION_PATCH(vkInstanceVersion)
    );

    // Surface
    try(vulkanPlatformCreateSurface(vkInstance, pPlatform, pWindow, pVkAllocator, &vkSurface));

    // Physical device
    U32 physicalDeviceCount = 0;
    VkResult result = vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, NULL);

    if (result != VK_SUCCESS) {
        error("Failed to get physical device count");
    }

    if (physicalDeviceCount == 0) {
        error("No physical devices found");
    }

    physicalDeviceCount = min(physicalDeviceCount, 32);
    VkPhysicalDevice physicalDevices[32];
    result = vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, physicalDevices);

    if (result == VK_INCOMPLETE) {
        alert("Enumerate physical devices was incomplete");
    } else if (result != VK_SUCCESS) {
        error("Failed to enumerate physical devices");
    }

    U32 maxScore = 0;
    U32 maxScoreIndex = -1;

    for (U32 i = 0; i < physicalDeviceCount; i++) {
        U32 score = _getPhysicalDeviceScore(physicalDevices[i]);

        if (score > 0 && score > maxScore) {
            maxScore = score;
            maxScoreIndex = i;
        }
    }

    if (maxScoreIndex == -1) {
        error("Failed to find suitable physical devices");
    }

    vkPhysicalDevice = physicalDevices[maxScoreIndex];

    logInfo("Vulkan renderer initialized");

    ok();
}

pub void vulkanRendererRelease(void) {
    logInfo("Vulkan renderer releasing");

    vkDestroyInstance(vkInstance, pVkAllocator);
}
