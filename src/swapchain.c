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
#include <stdlib.h>
#include <string.h>
#include <vulkanx/command_buffer.h>
#include <vulkanx/result.h>
#include <vulkanx/memory.h>
#include <vulkanx/swapchain.h>

// Select present mode.
static VkResult selectSwapchainPresentMode(
            VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
            VkPresentModeKHR* pPresentMode)
{
    // Allocate present modes.
    uint32_t presentModeCount = 0;
    VkPresentModeKHR* pPresentModes = NULL;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
            physicalDevice, surface, &presentModeCount, NULL);
    if (presentModeCount == 0) {
        return VK_ERROR_INCOMPATIBLE_DRIVER;
    }
    pPresentModes = 
        VKX_LOCAL_MALLOC_TYPE(VkPresentModeKHR, presentModeCount);
    {
        // Retrieve present modes.
        VkResult result = 
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                    physicalDevice, surface, &presentModeCount,
                    pPresentModes);
        if (VKX_IS_ERROR(result)) {
            // Free present modes.
            VKX_LOCAL_FREE(pPresentModes);
            return result;
        }
    }

    // Best present mode.
    VkPresentModeKHR bestPresentMode = 0;

    // Prefer mailbox.
    VkBool32 found = VK_FALSE;
    for (uint32_t presentModeIndex = 0;
                  presentModeIndex < presentModeCount;
                  presentModeIndex++) {
        if (pPresentModes[presentModeIndex] == VK_PRESENT_MODE_MAILBOX_KHR) {
            bestPresentMode = pPresentModes[presentModeIndex];
            found = VK_TRUE;
            break;
        }
    }
    // Prefer fifo (should always be available).
    if (found == VK_FALSE) {
        for (uint32_t presentModeIndex = 0;
                      presentModeIndex < presentModeCount;
                      presentModeIndex++) {
            if (pPresentModes[presentModeIndex] == VK_PRESENT_MODE_FIFO_KHR) {
                bestPresentMode = pPresentModes[presentModeIndex];
                found = VK_TRUE;
                break;
            }
        }
    }
    // Default.
    if (found == VK_FALSE) {
        bestPresentMode = pPresentModes[0];
    }

    // Free present modes.
    VKX_LOCAL_FREE(pPresentModes);

    // Done.
    *pPresentMode = bestPresentMode;
    return VK_SUCCESS;
}

// Select surface format.
static VkResult selectSwapchainSurfaceFormat(
            VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
            VkSurfaceFormatKHR* pSurfaceFormat)
{
    // Allocate surface formats.
    uint32_t surfaceFormatCount = 0;
    VkSurfaceFormatKHR* pSurfaceFormats = NULL;
    vkGetPhysicalDeviceSurfaceFormatsKHR(
            physicalDevice, surface, &surfaceFormatCount, NULL);
    pSurfaceFormats =
        VKX_LOCAL_MALLOC_TYPE(VkSurfaceFormatKHR, surfaceFormatCount);
    {
        // Retrieve surface formats.
        VkResult result = 
            vkGetPhysicalDeviceSurfaceFormatsKHR(
                    physicalDevice, 
                    surface,
                    &surfaceFormatCount, 
                    pSurfaceFormats);
        if (VKX_IS_ERROR(result)) {
            // Free surface formats.
            VKX_LOCAL_FREE(pSurfaceFormats);
            return result;
        }
    }

    // Best surface format.
    VkSurfaceFormatKHR bestSurfaceFormat = {
        .format = VK_FORMAT_UNDEFINED,
        .colorSpace = (VkColorSpaceKHR)0
    };

    // Prefer B8G8R8A8_UNORM and SRGB_NONLINEAR.
    for (uint32_t surfaceFormatIndex = 0;
                  surfaceFormatIndex < surfaceFormatCount;
                  surfaceFormatIndex++) {
        VkSurfaceFormatKHR thisSurfaceFormat = 
            pSurfaceFormats[surfaceFormatIndex];
        if ((thisSurfaceFormat.format == VK_FORMAT_UNDEFINED) ||
            (thisSurfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
             thisSurfaceFormat.colorSpace == 
             VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
            bestSurfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
            bestSurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
            break;
        }
    }

    // Prefer B8G8R8A8_UNORM.
    if (bestSurfaceFormat.format == VK_FORMAT_UNDEFINED) {
        for (uint32_t surfaceFormatIndex = 0;
                      surfaceFormatIndex < surfaceFormatCount;
                      surfaceFormatIndex++) {
            VkSurfaceFormatKHR thisSurfaceFormat = 
                pSurfaceFormats[surfaceFormatIndex];
            if (thisSurfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM) {
                bestSurfaceFormat = thisSurfaceFormat;
                break;
            }
        }
    }

    // Default.
    if (surfaceFormatCount != 0 &&
        bestSurfaceFormat.format == VK_FORMAT_UNDEFINED) {
        bestSurfaceFormat = pSurfaceFormats[0];
    }

    // Free surface formats.
    VKX_LOCAL_FREE(pSurfaceFormats);

    if (bestSurfaceFormat.format == VK_FORMAT_UNDEFINED) {
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }
    else {
        // Done.
        *pSurfaceFormat = bestSurfaceFormat;
        return VK_SUCCESS;
    }
}

// Create swapchain.
VkResult vkxCreateSwapchain(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            uint32_t graphicsQueueFamilyIndex,
            uint32_t presentQueueFamilyIndex,
            VkSurfaceKHR surface,
            VkExtent2D surfaceExtent,
            const VkAllocationCallbacks* pAllocator,
            VkxSwapchain* pSwapchain)
{
    assert(pSwapchain);
    memset(pSwapchain, 0, sizeof(VkxSwapchain));
    pSwapchain->physicalDevice = physicalDevice;
    pSwapchain->device = device;

    {
        // Select present mode.
        VkResult result = 
            selectSwapchainPresentMode(
                    physicalDevice, surface, &pSwapchain->presentMode);
        if (VKX_IS_ERROR(result)) {
            return result;
        }
    }

    {
        // Select surface format.
        VkResult result = 
            selectSwapchainSurfaceFormat(
                    physicalDevice, surface, &pSwapchain->surfaceFormat);
        if (VKX_IS_ERROR(result)) {
            return result;
        }
    }

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    {
        // Get surface capabilities.
        VkResult result = 
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                    physicalDevice, surface, &surfaceCapabilities);
        if (VKX_IS_ERROR(result)) {
            return result;
        }
    }

    // Select pre-transform.
    pSwapchain->preTransform = 
        surfaceCapabilities
        .supportedTransforms & 
              VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR 
            ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
            : surfaceCapabilities.currentTransform;

    // Select composite alpha.
    pSwapchain->compositeAlpha = 
        surfaceCapabilities
        .supportedCompositeAlpha &
              VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR
            ? VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR
            : 
        surfaceCapabilities
        .supportedCompositeAlpha & 
              VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
            ? VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
            :
        surfaceCapabilities
        .supportedCompositeAlpha & 
              VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
            ? VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
            : VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    // Select swapchain image usage.
    pSwapchain->imageUsage = 
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
        (surfaceCapabilities
         .supportedUsageFlags &
                (VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                 VK_IMAGE_USAGE_TRANSFER_DST_BIT));

    // Queue families unique?
    if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
        // Set queue family indices.
        pSwapchain->queueFamilyIndexCount = 2;
        pSwapchain->queueFamilyIndices[0] = graphicsQueueFamilyIndex;
        pSwapchain->queueFamilyIndices[1] = presentQueueFamilyIndex;
        // Set image sharing mode.
        pSwapchain->imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    }
    else {
        // Set queue family indices.
        pSwapchain->queueFamilyIndexCount = 1;
        pSwapchain->queueFamilyIndices[0] = graphicsQueueFamilyIndex;
        pSwapchain->queueFamilyIndices[1] = UINT32_MAX;
        // Set image sharing mode.
        pSwapchain->imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;       
    }

    // Get queues.
    vkGetDeviceQueue(
            device, graphicsQueueFamilyIndex, 0, 
            &pSwapchain->graphicsQueue);
    vkGetDeviceQueue(
            device, presentQueueFamilyIndex, 0, 
            &pSwapchain->presentQueue);
    
    // Create command pool.
    VkCommandPoolCreateInfo commandPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = graphicsQueueFamilyIndex
    };
    VkResult result = vkCreateCommandPool(
            device, &commandPoolCreateInfo, pAllocator, 
            &pSwapchain->commandPool);
    if (result != VK_SUCCESS) {
        return result;
    }

    // Delegate.
    return vkxRecreateSwapchain(
                surface, surfaceExtent, pAllocator, pSwapchain);
}

// Recreate swapchain.
VkResult vkxRecreateSwapchain(
            VkSurfaceKHR surface,
            VkExtent2D surfaceExtent,
            const VkAllocationCallbacks* pAllocator,
            VkxSwapchain* pSwapchain)
{
    assert(pSwapchain);
    VkPhysicalDevice physicalDevice = pSwapchain->physicalDevice;
    VkDevice device = pSwapchain->device;

    // Old swapchain.
    VkSwapchainKHR oldSwapchain = pSwapchain->swapchain;

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    {
        // Get surface capabilities.
        VkResult result =
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                    physicalDevice, surface, &surfaceCapabilities);
        if (VKX_IS_ERROR(result)) {
            vkxDestroySwapchain(pSwapchain, pAllocator);
            return result;
        }
    }

    // Set image extent.
    pSwapchain->imageExtent = surfaceCapabilities.currentExtent;

    // Undefined?
    if (pSwapchain->imageExtent.width == UINT32_MAX) {
        // Clamp surface extent width.
        if (surfaceExtent.width < surfaceCapabilities.minImageExtent.width) {
            surfaceExtent.width = surfaceCapabilities.minImageExtent.width;
        }
        if (surfaceExtent.width > surfaceCapabilities.maxImageExtent.width) {
            surfaceExtent.width = surfaceCapabilities.maxImageExtent.width;
        }
        // Clamp surface extent height.
        if (surfaceExtent.height < surfaceCapabilities.minImageExtent.height) {
            surfaceExtent.height = surfaceCapabilities.minImageExtent.height;
        }
        if (surfaceExtent.height > surfaceCapabilities.maxImageExtent.height) {
            surfaceExtent.height = surfaceCapabilities.maxImageExtent.height;
        }
        // Set image extent.
        pSwapchain->imageExtent = surfaceExtent;
    }

    // Minimum image count.
    uint32_t minImageCount = 
        surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 &&
        minImageCount > surfaceCapabilities.maxImageCount) {
        minImageCount = surfaceCapabilities.maxImageCount;
    }

    // Swapchain create info.
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .surface = surface,
        .minImageCount = minImageCount,
        .imageFormat = pSwapchain->surfaceFormat.format,
        .imageColorSpace = pSwapchain->surfaceFormat.colorSpace,
        .imageExtent = pSwapchain->imageExtent,
        .imageArrayLayers = 1,
        .imageUsage = pSwapchain->imageUsage,
        .imageSharingMode = pSwapchain->imageSharingMode,
        .queueFamilyIndexCount = pSwapchain->queueFamilyIndexCount,
        .pQueueFamilyIndices = &pSwapchain->queueFamilyIndices[0],
        .preTransform = pSwapchain->preTransform,
        .compositeAlpha = pSwapchain->compositeAlpha,
        .presentMode = pSwapchain->presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = oldSwapchain
    };
    {
        // Create swapchain.
        VkResult result = 
            vkCreateSwapchainKHR(
                    device,
                    &swapchainCreateInfo, pAllocator,
                    &pSwapchain->swapchain);
        if (VKX_IS_ERROR(result)) {
            pSwapchain->swapchain = oldSwapchain;
            vkxDestroySwapchain(pSwapchain, pAllocator);
            return result;
        }
    }

    for (uint32_t imageIndex = 0; imageIndex < pSwapchain->imageCount;
                  imageIndex++) {
        // Destroy image views.
        vkDestroyImageView(
                device,
                pSwapchain->pImageViews[imageIndex], pAllocator);
        // Destroy acquired semaphores.
        vkDestroySemaphore(
                device,
                pSwapchain->pAcquiredSemaphores[imageIndex], pAllocator);
        // Destroy released semaphores.
        vkDestroySemaphore(
                device,
                pSwapchain->pReleasedSemaphores[imageIndex], pAllocator);
        // Destroy fences.
        vkDestroyFence(
                device,
                pSwapchain->pFences[imageIndex], pAllocator);
    }
    // Destroy next acquired semaphore.
    vkDestroySemaphore(
            device, pSwapchain->nextAcquiredSemaphore, pAllocator);
    // Destroy next released semaphore.
    vkDestroySemaphore(
            device, pSwapchain->nextReleasedSemaphore, pAllocator);
    // Free command buffers.
    vkFreeCommandBuffers(
            device,
            pSwapchain->commandPool, pSwapchain->imageCount,
            pSwapchain->pCommandBuffers);
    // Destroy old swapchain.
    vkDestroySwapchainKHR(
            device, oldSwapchain, pAllocator);

    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(
            device, pSwapchain->swapchain, &imageCount, NULL);
    if (pSwapchain->imageCount != imageCount) {
        pSwapchain->imageCount = imageCount;
        // Reallocate images.
        pSwapchain->pImages = 
                realloc(pSwapchain->pImages, 
                        imageCount * sizeof(VkImage));
        // Reallocate image views.
        pSwapchain->pImageViews = 
                realloc(pSwapchain->pImageViews, 
                        imageCount * sizeof(VkImageView));
        // Reallocate indices.
        pSwapchain->pIndices = 
                realloc(pSwapchain->pIndices, 
                        imageCount * sizeof(uint32_t));
        // Reallocate acquired semaphores. 
        pSwapchain->pAcquiredSemaphores = 
                realloc(pSwapchain->pAcquiredSemaphores, 
                        imageCount * sizeof(VkSemaphore));
        // Reallocate released semaphores. 
        pSwapchain->pReleasedSemaphores = 
                realloc(pSwapchain->pReleasedSemaphores, 
                        imageCount * sizeof(VkSemaphore));
        // Reallocate fences.
        pSwapchain->pFences = 
                realloc(pSwapchain->pFences, 
                        imageCount * sizeof(VkFence));
        // Reallocate command buffers.
        pSwapchain->pCommandBuffers = 
                realloc(pSwapchain->pCommandBuffers,
                        imageCount * sizeof(VkCommandBuffer));
    }
    // Nullify images.
    memset(pSwapchain->pImages, 0, imageCount * sizeof(VkImage));
    // Nullify image views.
    memset(pSwapchain->pImageViews, 0, imageCount * sizeof(VkImageView));
    // Nullify/invalidate indices.
    memset(pSwapchain->pIndices, 0xFF, imageCount * sizeof(uint32_t));
    // Nullify acquired semaphores.
    memset(pSwapchain->pAcquiredSemaphores, 
           0, imageCount * sizeof(VkSemaphore));
    // Nullify released semaphores.
    memset(pSwapchain->pReleasedSemaphores, 
           0, imageCount * sizeof(VkSemaphore));
    // Nullify temporary acquired semaphore.
    pSwapchain->nextAcquiredSemaphore = VK_NULL_HANDLE;
    // Nullify temporary released semaphore.
    pSwapchain->nextReleasedSemaphore = VK_NULL_HANDLE;
    // Nullify fences.
    memset(pSwapchain->pFences, 0, imageCount * sizeof(VkFence));
    // Nullify command buffers.
    memset(pSwapchain->pCommandBuffers, 
           0, imageCount * sizeof(VkCommandBuffer));
    
    {
        // Get swapchain images.
        VkResult result = 
            vkGetSwapchainImagesKHR(
                    device,
                    pSwapchain->swapchain,
                    &pSwapchain->imageCount,
                    pSwapchain->pImages);
        if (VKX_IS_ERROR(result)) {
            vkxDestroySwapchain(pSwapchain, pAllocator);
            return result;
        }
    }

    // Image view create info.
    VkImageViewCreateInfo imageViewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .image = VK_NULL_HANDLE, // Uninitialized 
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = pSwapchain->surfaceFormat.format,
        .components = {
            .r = VK_COMPONENT_SWIZZLE_R,
            .g = VK_COMPONENT_SWIZZLE_G,
            .b = VK_COMPONENT_SWIZZLE_B,
            .a = VK_COMPONENT_SWIZZLE_A
        },
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0, .levelCount = 1,
            .baseArrayLayer = 0, .layerCount = 1
        }
    };

    for (uint32_t imageIndex = 0; imageIndex < pSwapchain->imageCount;
                  imageIndex++) {
        // Create swapchain image view.
        imageViewCreateInfo.image = pSwapchain->pImages[imageIndex];
        VkResult result = 
            vkCreateImageView(
                    device,
                    &imageViewCreateInfo, pAllocator,
                    &pSwapchain->pImageViews[imageIndex]);
        if (VKX_IS_ERROR(result)) {
            vkxDestroySwapchain(pSwapchain, pAllocator);
            return result;
        }
    }

    // Create semaphores.
    uint32_t semaphoreCounts[4] = {imageCount, imageCount, 1, 1};
    VkSemaphore* pSemaphores[4] = {
        pSwapchain->pAcquiredSemaphores,
        pSwapchain->pReleasedSemaphores,
        &pSwapchain->nextAcquiredSemaphore,
        &pSwapchain->nextReleasedSemaphore
    };
    for (uint32_t paramIndex = 0; paramIndex < 4; 
                  paramIndex++) {
        VkResult result =
            vkxCreateDefaultSemaphores(
                    device, semaphoreCounts[paramIndex], pAllocator, 
                    pSemaphores[paramIndex]);
        if (VKX_IS_ERROR(result)) {
            vkxDestroySwapchain(pSwapchain, pAllocator);
            return result;
        }
    }

    // Create fences.
    for (uint32_t imageIndex = 0; imageIndex < imageCount;
                  imageIndex++) {
        VkFenceCreateInfo fenceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT // Default signaled
        };
        VkResult result = 
            vkCreateFence(
                    device, 
                    &fenceCreateInfo, pAllocator,
                    &pSwapchain->pFences[imageIndex]);
        if (VKX_IS_ERROR(result)) {
            vkxDestroySwapchain(pSwapchain, pAllocator);
            return result;
        }
    }

    // Allocate command buffers.
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = pSwapchain->commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = pSwapchain->imageCount
    };
    VkResult result = vkAllocateCommandBuffers(
            pSwapchain->device, &commandBufferAllocateInfo,
            pSwapchain->pCommandBuffers);
    if (VKX_IS_ERROR(result)) {
        vkxDestroySwapchain(pSwapchain, pAllocator);
        return result;
    }

    // Nullify active state.
    pSwapchain->activeIndex = UINT32_MAX;
    pSwapchain->activeAcquiredSemaphore = VK_NULL_HANDLE;
    pSwapchain->activeReleasedSemaphore = VK_NULL_HANDLE;
    pSwapchain->activeFence = VK_NULL_HANDLE;
    pSwapchain->activeCommandBuffer = VK_NULL_HANDLE;

    return VK_SUCCESS;
}

void vkxDestroySwapchain(
            VkxSwapchain* pSwapchain,
            const VkAllocationCallbacks* pAllocator)
{
    if (pSwapchain) {
        VkDevice device = pSwapchain->device;
        for (uint32_t imageIndex = 0; imageIndex < pSwapchain->imageCount; 
                      imageIndex++) {
            // Destroy image views.
            vkDestroyImageView(
                    device,
                    pSwapchain->pImageViews[imageIndex], pAllocator);
            // Destroy acquired semaphores.
            vkDestroySemaphore(
                    device,
                    pSwapchain->pAcquiredSemaphores[imageIndex], pAllocator);
            // Destroy released semaphores.
            vkDestroySemaphore(
                    device,
                    pSwapchain->pReleasedSemaphores[imageIndex], pAllocator);
            // Destroy fences.
            vkDestroyFence(
                    device,
                    pSwapchain->pFences[imageIndex], pAllocator);
        }
        // Destroy next acquired semaphore.
        vkDestroySemaphore(
                device, pSwapchain->nextAcquiredSemaphore, pAllocator);
        // Destroy next released semaphore.
        vkDestroySemaphore(
                device, pSwapchain->nextReleasedSemaphore, pAllocator);
        // Destroy swapchain.
        vkDestroySwapchainKHR(
                device,
                pSwapchain->swapchain, pAllocator);
        // Free command buffers.
        free(pSwapchain->pCommandBuffers);
        // Free fences.
        free(pSwapchain->pFences);
        // Free released semaphores.
        free(pSwapchain->pReleasedSemaphores);
        // Free acquired semaphores.
        free(pSwapchain->pAcquiredSemaphores);
        // Free indices.
        free(pSwapchain->pIndices);
        // Free image views.
        free(pSwapchain->pImageViews);
        // Free images.
        free(pSwapchain->pImages);
        // Nullify.
        memset(pSwapchain, 0, sizeof(VkxSwapchain));
    }
}

static void swapSemaphores(VkSemaphore* pSem1, VkSemaphore* pSem2)
{
    VkSemaphore tmp = *pSem1;
    *pSem1 = *pSem2;
    *pSem2 = tmp;
}

VkResult vkxSwapchainAcquireNextImage(
            VkxSwapchain* pSwapchain, uint64_t timeout)
{
    // Acquire next image.
    uint32_t nextImageIndex = 0;
    VkResult result = vkAcquireNextImageKHR(
            pSwapchain->device,
            pSwapchain->swapchain, timeout,
            pSwapchain->nextAcquiredSemaphore, VK_NULL_HANDLE, 
            &nextImageIndex);
    if (result == VK_SUCCESS) {
        // Swap semaphores.
        swapSemaphores(
            &pSwapchain->nextAcquiredSemaphore,
            &pSwapchain->pAcquiredSemaphores[nextImageIndex]);
        swapSemaphores(
            &pSwapchain->nextReleasedSemaphore,
            &pSwapchain->pReleasedSemaphores[nextImageIndex]);

        // Shuffle indices.
        for (uint32_t imageIndex = 1; imageIndex < pSwapchain->imageCount; 
                      imageIndex++) {
            pSwapchain->pIndices[imageIndex] = 
            pSwapchain->pIndices[imageIndex - 1];
        }
        pSwapchain->pIndices[0] = nextImageIndex;

        // Wait until image is done rendering.
        vkWaitForFences(
                pSwapchain->device, 
                1, &pSwapchain->pFences[nextImageIndex], VK_TRUE, UINT64_MAX);
        vkResetFences(
                pSwapchain->device,
                1, &pSwapchain->pFences[nextImageIndex]);

        // Update active state. 
        pSwapchain->activeIndex = nextImageIndex;
        pSwapchain->activeAcquiredSemaphore = 
            pSwapchain->pAcquiredSemaphores[nextImageIndex];
        pSwapchain->activeReleasedSemaphore = 
            pSwapchain->pReleasedSemaphores[nextImageIndex];
        pSwapchain->activeFence = 
            pSwapchain->pFences[nextImageIndex];
        pSwapchain->activeCommandBuffer = 
            pSwapchain->pCommandBuffers[nextImageIndex];
    }

    return result;
}

VkResult vkxSwapchainSubmit(
            VkxSwapchain* pSwapchain)
{
    VkPipelineStageFlags waitDstStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &pSwapchain->activeAcquiredSemaphore,
        .pWaitDstStageMask = &waitDstStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &pSwapchain->activeCommandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &pSwapchain->activeReleasedSemaphore
    };
    VkResult result = vkQueueSubmit(
            pSwapchain->graphicsQueue, 1, &submitInfo, 
            pSwapchain->activeFence);
    return result;
}

VkResult vkxSwapchainPresent(
            VkxSwapchain* pSwapchain,
            uint32_t moreWaitSemaphoreCount,
            const VkSemaphore* pMoreWaitSemaphores)
{
    // Initialize wait semaphores.
    VkSemaphore* pWaitSemaphores = 
        VKX_LOCAL_MALLOC_TYPE(VkSemaphore, moreWaitSemaphoreCount + 1);
    pWaitSemaphores[0] = pSwapchain->activeReleasedSemaphore;
    memcpy(pWaitSemaphores + 1, pMoreWaitSemaphores, 
           sizeof(VkSemaphore) * moreWaitSemaphoreCount);

    // Present info.
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = moreWaitSemaphoreCount + 1,
        .pWaitSemaphores = pWaitSemaphores,
        .swapchainCount = 1,
        .pSwapchains = &pSwapchain->swapchain,
        .pImageIndices = &pSwapchain->pIndices[0],
        .pResults = NULL
    };

    // Present.
    VkResult result = 
        vkQueuePresentKHR(pSwapchain->presentQueue, &presentInfo);

    // Free wait semaphores.
    VKX_LOCAL_FREE(pWaitSemaphores);

    return result;
}
