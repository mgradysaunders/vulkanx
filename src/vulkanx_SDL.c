/* Copyright (c) 2019-20 M. Grady Saunders
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *   1. Redistributions of source code must retain the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer.
 * 
 *   2. Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials
 *      provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*-*-*-*-*-*-*/
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <vulkanx_SDL.h>

static char* mallocAndCopyString(const char* str)
{
    if (str == NULL) {
        return NULL;
    }
    else {
        char* dup = (char*)malloc(strlen(str) + 1);
        memcpy(dup, str, strlen(str) + 1);
        return dup;
    }
}

// Is physical device okay for SDL?
static VkBool32 isPhysicalDeviceOkayForSDL(
                VkPhysicalDevice physicalDevice, void* pUserData)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
            physicalDevice, &queueFamilyCount, NULL);
    for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount;
                  queueFamilyIndex++) {
        // It is okay if it has a queue family which supports present
        // operations.
        VkBool32 supported = VK_FALSE;
        VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(
                physicalDevice, queueFamilyIndex, 
                *(VkSurfaceKHR*)pUserData, &supported);
        if (result == VK_SUCCESS && supported == VK_TRUE) {
            return VK_TRUE;
        }
    }
    return VK_FALSE;
}

void vkxCreateSDLWindowOrExit(
        VkxSDLWindowCreateInfo* pCreateInfo,
        VkxSDLWindow* pWindow)
{
    assert(pCreateInfo);
    assert(pWindow);

    // Create SDL window.
    pWindow->window = SDL_CreateWindow(
            pCreateInfo->pInstanceCreateInfo->pApplicationName,
            pCreateInfo->positionX,
            pCreateInfo->positionY,
            pCreateInfo->sizeX,
            pCreateInfo->sizeY,
            pCreateInfo->windowFlags | SDL_WINDOW_VULKAN);
    if (!pWindow->window) {
        fprintf(stderr, 
                "failed to create SDL window\n"
                "SDL_GetError(): %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Get SDL instance extension count.
    unsigned int requiredExtensionCount = 0;
    uint32_t requestedExtensionCount = 
            pCreateInfo->pInstanceCreateInfo->requestedExtensionCount;
    if (!SDL_Vulkan_GetInstanceExtensions(
                pWindow->window, &requiredExtensionCount, NULL)) {
        fprintf(stderr, "failed to get SDL Vulkan instance extensions\n");
        exit(EXIT_FAILURE);
    }
    // Allocate space for SDL extensions and user-requested extensions, 
    // then get SDL extension names.
    uint32_t extensionCount = requiredExtensionCount;
    const char** ppExtensionNames =
            VKX_LOCAL_MALLOC_TYPE(const char*, 
            requiredExtensionCount + requestedExtensionCount);
    if (!SDL_Vulkan_GetInstanceExtensions(
                pWindow->window, &extensionCount, ppExtensionNames)) {
        fprintf(stderr, "failed to get SDL Vulkan instance extensions\n");
        exit(EXIT_FAILURE);
    }

    // Add user-requested extension names that aren't duplicates of 
    // SDL extension names.
    for (uint32_t requestIndex = 0; requestIndex < requestedExtensionCount;
                  requestIndex++) {
        const char* pRequestedExtensionName = 
                    pCreateInfo->pInstanceCreateInfo->
                    ppRequestedExtensionNames[requestIndex];
        VkBool32 alreadyExists = VK_FALSE;
        for (const char** ppExtensionName = ppExtensionNames;
                          ppExtensionName < ppExtensionNames + extensionCount;
                          ppExtensionName++) {
            if (!strcmp(*ppExtensionName, pRequestedExtensionName)) {
                alreadyExists = VK_TRUE;
                break;
            }
        }
        if (alreadyExists == VK_FALSE) {
            ppExtensionNames[extensionCount++] = pRequestedExtensionName;
        }
    } 

    // User-requested layers.
    uint32_t layerCount = 
            pCreateInfo->pInstanceCreateInfo->requestedLayerCount;
    const char* const* ppLayerNames = 
            pCreateInfo->pInstanceCreateInfo->ppRequestedLayerNames;

    // Allocate layer and extension enabled flags.
    VkBool32* pLayersEnabled = 
            VKX_LOCAL_MALLOC_TYPE(VkBool32, layerCount);
    VkBool32* pExtensionsEnabled = 
            VKX_LOCAL_MALLOC_TYPE(VkBool32, extensionCount);
    {
        // Create instance.
        VkxInstanceCreateInfo instanceCreateInfo;
        memcpy(&instanceCreateInfo, 
                pCreateInfo->pInstanceCreateInfo, 
                sizeof(VkxInstanceCreateInfo));
        instanceCreateInfo.requestedExtensionCount = extensionCount;
        instanceCreateInfo.ppRequestedExtensionNames = ppExtensionNames;
        VkResult result = vkxCreateInstance(
                &instanceCreateInfo, NULL,
                pLayersEnabled, 
                pExtensionsEnabled, 
                &pWindow->instance);
        if (result != VK_SUCCESS) {
            fprintf(stderr, 
                    "failed to create Vulkan instance (%s)\n", 
                    vkxResultName(result));
            exit(EXIT_FAILURE);
        }
    }

    // Initialize enabled layers.
    pWindow->enabledLayerCount = 0;
    pWindow->ppEnabledLayerNames = 
            (char**)malloc(sizeof(char*) * extensionCount);
    for (uint32_t layerIndex = 0; layerIndex < layerCount;
                  layerIndex++) {
        if (pLayersEnabled[layerIndex] == VK_TRUE) {
            pWindow->ppEnabledLayerNames[pWindow->enabledLayerCount] =
                     mallocAndCopyString(ppLayerNames[layerIndex]);
            pWindow->enabledLayerCount++;
        }
    }

    // Initialize enabled extensions.
    pWindow->enabledExtensionCount = 0;
    pWindow->ppEnabledExtensionNames = 
            (char**)malloc(sizeof(char*) * extensionCount);
    for (uint32_t extensionIndex = 0; extensionIndex < extensionCount;
                  extensionIndex++) {
        if (pExtensionsEnabled[extensionIndex] == VK_TRUE) {
            pWindow->ppEnabledExtensionNames[pWindow->enabledExtensionCount] =
                     mallocAndCopyString(ppExtensionNames[extensionIndex]);
            pWindow->enabledExtensionCount++;
            continue; // Skip ahead.
        }

        // If extension is required but not enabled, report error and exit.
        if (extensionIndex < requiredExtensionCount) {
            fprintf(stderr, 
                    "can't enable Vulkan extension \"%s\" required by SDL\n",
                    ppExtensionNames[extensionIndex]);
            exit(EXIT_FAILURE);
        }
    }

    // Free layer enabled flags.
    VKX_LOCAL_FREE(pLayersEnabled);

    // Free extension enabled flags.
    VKX_LOCAL_FREE(pExtensionsEnabled);

    // Free extension name pointers.
    VKX_LOCAL_FREE(ppExtensionNames);

    // Create surface.
    if (!SDL_Vulkan_CreateSurface(
                pWindow->window, 
                pWindow->instance, 
                &pWindow->swapchainSurface)) {
        fprintf(stderr, "failed to create Vulkan swapchain surface\n");
        exit(EXIT_FAILURE);
    }

    {
        // Swapchain extension name.
        const char* swapchainExtensionName = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
        // Select physical device which supports present 
        // operations.
        VkxPhysicalDeviceSelectInfo physicalDeviceSelectInfo = {
            .pRequestedName = NULL,
            .pRequestedFeatures = NULL,
            .pIsPhysicalDeviceOkay = isPhysicalDeviceOkayForSDL,
            .pUserData = &pWindow->swapchainSurface
        };
        // Use 1 graphics queue family, with up to 4 queues,
        // which supports present operations.
        VkxDeviceQueueFamilyCreateInfo queueFamilyCreateInfo = {
            .queueFlags = VK_QUEUE_GRAPHICS_BIT,
            .queueCount = 4,
            .minQueueCount = 1,
            .useEqualPriority = VK_FALSE,
            .presentSurface = pWindow->swapchainSurface,
            .commandPoolCount = pCreateInfo->overrideCommandPoolCount,
            .pCommandPoolCreateFlags = 
                pCreateInfo->pOverrideCommandPoolCreateFlags
        };
        // If no command pools, create 1 command pool with no special
        // flags by default.
        VkCommandPoolCreateFlags defaultCommandPoolCreateFlags = 0;
        if (queueFamilyCreateInfo.commandPoolCount == 0) {
            queueFamilyCreateInfo.commandPoolCount = 1;
            queueFamilyCreateInfo.pCommandPoolCreateFlags = 
                &defaultCommandPoolCreateFlags;
        }
        // Create device.
        VkxDeviceCreateInfo deviceCreateInfo = {
            .pSelectInfo = &physicalDeviceSelectInfo,
            .queueFamilyCreateInfoCount = 1,
            .pQueueFamilyCreateInfos = &queueFamilyCreateInfo,
            .enabledExtensionCount = 1,
            .ppEnabledExtensionNames = &swapchainExtensionName
        };
        VkResult result = vkxCreateDevice(
                pWindow->instance, &deviceCreateInfo, NULL,
                &pWindow->device);
        if (result != VK_SUCCESS) {
            fprintf(stderr, 
                    "failed to create Vulkan device (%s)\n",
                    vkxResultName(result));
            exit(EXIT_FAILURE);
        }
    }

    {
        // Create swapchain.
        int drawableWidth = 0;
        int drawableHeight = 0;
        SDL_Vulkan_GetDrawableSize(
                pWindow->window, 
                &drawableWidth, 
                &drawableHeight);
        VkExtent2D surfaceExtent = {
            .width = (uint32_t)drawableWidth,
            .height = (uint32_t)drawableHeight
        };
        VkResult result = vkxCreateSwapchain(
                pWindow->device.physicalDevice,
                pWindow->device.device,
                pWindow->device.pQueueFamilies[0].queueFamilyIndex,
                pWindow->device.pQueueFamilies[0].queueFamilyIndex,
                pWindow->swapchainSurface,
                surfaceExtent, 
                NULL,
                &pWindow->swapchain);
        if (result != VK_SUCCESS) {
            fprintf(stderr, 
                    "failed to create Vulkan swapchain (%s)\n",
                    vkxResultName(result));
            exit(EXIT_FAILURE);
        }
    }

    pWindow->pSwapchainIndices = 
        (uint32_t*)malloc(
                sizeof(uint32_t) * pWindow->swapchain.imageCount);
    for (uint32_t imageIndex = 0; imageIndex < pWindow->swapchain.imageCount;
                  imageIndex++) {
        pWindow->pSwapchainIndices[imageIndex] = pWindow->swapchain.imageCount;
    }

    pWindow->pSwapchainAvailableSemaphores = 
        (VkSemaphore*)malloc(
                sizeof(VkSemaphore) * pWindow->swapchain.imageCount);
    memset(pWindow->pSwapchainAvailableSemaphores, 0,
                sizeof(VkSemaphore) * pWindow->swapchain.imageCount);

    VkSemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0
    };
    for (uint32_t imageIndex = 0; imageIndex < pWindow->swapchain.imageCount;
                  imageIndex++) {
        VkResult result = vkCreateSemaphore(
                pWindow->device.device,
                &semaphoreCreateInfo, NULL,
                &pWindow->pSwapchainAvailableSemaphores[imageIndex]);
        if (result != VK_SUCCESS) {
            fprintf(stderr, "failed to create swapchain semaphores\n");
            exit(EXIT_FAILURE);
        }
    }
    {
        VkResult result = vkCreateSemaphore(
                pWindow->device.device,
                &semaphoreCreateInfo, NULL,
                &pWindow->swapchainAvailableSemaphore);
        if (result != VK_SUCCESS) {
            fprintf(stderr, "failed to create swapchain semaphores\n");
            exit(EXIT_FAILURE);
        }
    }
}

void vkxDestroySDLWindow(VkxSDLWindow* pWindow)
{
    if (pWindow) {
        // Free layer names. 
        for (uint32_t nameIndex = 0; 
                      nameIndex < pWindow->enabledLayerCount; nameIndex++)
            free(pWindow->ppEnabledLayerNames[nameIndex]);
        free(pWindow->ppEnabledLayerNames);

        // Free extension names.
        for (uint32_t nameIndex = 0; 
                      nameIndex < pWindow->enabledExtensionCount; nameIndex++)
            free(pWindow->ppEnabledExtensionNames[nameIndex]);
        free(pWindow->ppEnabledExtensionNames);

        // Free swapchain semaphores and indices.
        for (uint32_t imageIndex = 0; 
                      imageIndex < pWindow->swapchain.imageCount; imageIndex++)
            vkDestroySemaphore(
                    pWindow->device.device, 
                    pWindow->pSwapchainAvailableSemaphores[imageIndex],
                    NULL);
        vkDestroySemaphore(
                pWindow->device.device, 
                pWindow->swapchainAvailableSemaphore,
                NULL);
        free(pWindow->pSwapchainAvailableSemaphores);
        free(pWindow->pSwapchainIndices);

        // Destroy swapchain.
        vkxDestroySwapchain(
                pWindow->device.device, &pWindow->swapchain, NULL);

        // Destroy swapchain surface.
        vkDestroySurfaceKHR(
                pWindow->instance, pWindow->swapchainSurface, NULL);

        // Destroy device.
        vkxDestroyDevice(&pWindow->device, NULL);

        // Destroy instance.
        vkDestroyInstance(pWindow->instance, NULL);

        // Destroy window.
        SDL_DestroyWindow(pWindow->window);

        // Nullify.
        memset(pWindow, 0, sizeof(VkxSDLWindow));
    }
}

#if 0
VkResult vkxSDLWindowRecreateSwapchain(VkxSDLWindow* pWindow)
{
    // Recreate swapchain.
    int drawableWidth = 0;
    int drawableHeight = 0;
    SDL_Vulkan_GetDrawableSize(
            pWindow->window, 
            &drawableWidth, 
            &drawableHeight);
    VkExtent2D surfaceExtent = {
        .width = (uint32_t)drawableWidth,
        .height = (uint32_t)drawableHeight
    };
    VkResult result = vkxRecreateSwapchain(
            pWindow->device.physicalDevice,
            pWindow->device.device,
            pWindow->swapchainSurface,
            surfaceExtent,
            NULL,
            &pWindow->swapchain);
    if (result != VK_SUCCESS) {
        return result;
    }
    // TODO
    return VK_SUCCESS;
}
#endif

VkResult vkxSDLWindowAcquireNextImage(VkxSDLWindow* pWindow, uint64_t timeout)
{
    uint32_t nextImageIndex = 0;
    VkResult result = vkAcquireNextImageKHR(
            pWindow->device.device,
            pWindow->swapchain.swapchain,
            timeout,
            pWindow->swapchainAvailableSemaphore,
            VK_NULL_HANDLE, &nextImageIndex);
    if (result == VK_SUCCESS) {
        // Swap semaphores.
        VkSemaphore* semaphore1 = &pWindow->swapchainAvailableSemaphore;
        VkSemaphore* semaphore2 = 
                &pWindow->pSwapchainAvailableSemaphores[nextImageIndex];
        VkSemaphore tmp = *semaphore1;
        *semaphore1 = *semaphore2;
        *semaphore2 = tmp;

        // Shuffle indices.
        uint32_t imageCount = pWindow->swapchain.imageCount;
        for (uint32_t imageIndex = 1; imageIndex < imageCount; 
                      imageIndex++) {
            pWindow->pSwapchainIndices[imageIndex] =  
            pWindow->pSwapchainIndices[imageIndex - 1];
        }
        pWindow->pSwapchainIndices[0] = nextImageIndex;
    }
    return result;
}

VkResult vkxSDLWindowPresent(VkxSDLWindow* pWindow)
{
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = NULL, // TODO
        .swapchainCount = 1,
        .pSwapchains = &pWindow->swapchain.swapchain,
        .pImageIndices = pWindow->pSwapchainIndices,
        .pResults = NULL
    };

    return vkQueuePresentKHR(
            pWindow->device.pQueueFamilies[0].pQueues[0], &presentInfo);
}