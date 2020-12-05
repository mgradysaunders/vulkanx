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
#include <vulkanx/result.h>
#include <vulkanx/memory.h>
#include <vulkanx/swapchain.h>

// Select present mode.
static
VkResult selectSwapchainPresentMode(
            VkPhysicalDevice physicalDevice, 
            VkSurfaceKHR surface,
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
        (VkPresentModeKHR*)VKX_LOCAL_MALLOC(
                sizeof(VkPresentModeKHR) * presentModeCount);
    {
        // Retrieve present modes.
        VkResult result = 
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                    physicalDevice, surface, 
                    &presentModeCount,
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
static 
VkResult selectSwapchainSurfaceFormat(
            VkPhysicalDevice physicalDevice, 
            VkSurfaceKHR surface,
            VkSurfaceFormatKHR* pSurfaceFormat)
{
    // Allocate surface formats.
    uint32_t surfaceFormatCount = 0;
    VkSurfaceFormatKHR* pSurfaceFormats = NULL;
    vkGetPhysicalDeviceSurfaceFormatsKHR(
            physicalDevice, 
            surface,
            &surfaceFormatCount, 
            NULL);
    pSurfaceFormats =
        (VkSurfaceFormatKHR*)VKX_LOCAL_MALLOC(
                sizeof(VkSurfaceFormatKHR) * surfaceFormatCount);
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

    {
        // Select present mode.
        VkResult result = 
            selectSwapchainPresentMode(
                    physicalDevice, 
                    surface, 
                    &pSwapchain->presentMode);
        if (VKX_IS_ERROR(result)) {
            return result;
        }
    }

    {
        // Select surface format.
        VkResult result = 
            selectSwapchainSurfaceFormat(
                    physicalDevice, 
                    surface, 
                    &pSwapchain->surfaceFormat);
        if (VKX_IS_ERROR(result)) {
            return result;
        }
    }

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    {
        // Get surface capabilities.
        VkResult result = 
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                    physicalDevice,
                    surface,
                    &surfaceCapabilities);
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

    // Delegate.
    return vkxRecreateSwapchain(
                physicalDevice,
                device,
                surface,
                surfaceExtent,
                pAllocator,
                pSwapchain);
}

// Recreate swapchain.
VkResult vkxRecreateSwapchain(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VkSurfaceKHR surface,
            VkExtent2D surfaceExtent,
            const VkAllocationCallbacks* pAllocator,
            VkxSwapchain* pSwapchain)
{
    assert(pSwapchain);

    // Old swapchain.
    VkSwapchainKHR oldSwapchain = pSwapchain->swapchain;

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    {
        // Get surface capabilities.
        VkResult result =
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                    physicalDevice,
                    surface,
                    &surfaceCapabilities);
        if (VKX_IS_ERROR(result)) {
            // Destroy swapchain.
            vkxDestroySwapchain(device, pSwapchain, pAllocator);
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
            // Destroy swapchain.
            pSwapchain->swapchain = oldSwapchain;
            vkxDestroySwapchain(device, pSwapchain, pAllocator);
            return result;
        }
    }

    for (uint32_t imageIndex = 0;
                  imageIndex < pSwapchain->imageCount;
                  imageIndex++) {

        // Destroy old swapchain image views.
        vkDestroyImageView(
                device,
                pSwapchain->pImageViews[imageIndex], pAllocator);
    }

    // Destroy old swapchain.
    vkDestroySwapchainKHR(
            device,
            oldSwapchain, pAllocator);

    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(
            device,
            pSwapchain->swapchain,
            &imageCount, NULL);
    if (pSwapchain->imageCount != imageCount) {
        pSwapchain->imageCount = imageCount;

        // Reallocate swapchain images.
        pSwapchain->pImages = 
            (VkImage*)realloc(
                    pSwapchain->pImages, 
                    sizeof(VkImage) * imageCount);

        // Reallocate swapchain image views.
        pSwapchain->pImageViews = 
            (VkImageView*)realloc(
                    pSwapchain->pImageViews, 
                    sizeof(VkImageView) * imageCount);
    }
    for (uint32_t imageIndex = 0;
                  imageIndex < pSwapchain->imageCount;
                  imageIndex++) {

        // Nullify swapchain images.
        pSwapchain->pImages[imageIndex] = VK_NULL_HANDLE;

        // Nullify swapchain image views.
        pSwapchain->pImageViews[imageIndex] = VK_NULL_HANDLE;
    }

    {
        // Get swapchain images.
        VkResult result = 
            vkGetSwapchainImagesKHR(
                    device,
                    pSwapchain->swapchain,
                    &pSwapchain->imageCount,
                    pSwapchain->pImages);
        if (VKX_IS_ERROR(result)) {
            // Destroy swapchain.
            vkxDestroySwapchain(device, pSwapchain, pAllocator);
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

    for (uint32_t imageIndex = 0;
                  imageIndex < pSwapchain->imageCount;
                  imageIndex++) {
        // Create swapchain image view.
        imageViewCreateInfo.image = pSwapchain->pImages[imageIndex];
        VkResult result = 
            vkCreateImageView(
                    device,
                    &imageViewCreateInfo, pAllocator,
                    &pSwapchain->pImageViews[imageIndex]);
        if (VKX_IS_ERROR(result)) {
            // Destroy swapchain.
            pSwapchain->pImageViews[imageIndex] = VK_NULL_HANDLE;
            vkxDestroySwapchain(device, pSwapchain, pAllocator);
            return result;
        }
    }

    return VK_SUCCESS;
}

void vkxDestroySwapchain(
            VkDevice device,
            VkxSwapchain* pSwapchain,
            const VkAllocationCallbacks* pAllocator)
{
    if (pSwapchain) {

        for (uint32_t imageIndex = 0;
                      imageIndex < pSwapchain->imageCount; 
                      imageIndex++) {

            // Destroy swapchain image views.
            vkDestroyImageView(
                    device,
                    pSwapchain->pImageViews[imageIndex],
                    pAllocator);
        }

        // Destroy swapchain.
        vkDestroySwapchainKHR(
                device,
                pSwapchain->swapchain, 
                pAllocator);

        // Free swapchain image views.
        free(pSwapchain->pImageViews);

        // Free swapchain images.
        free(pSwapchain->pImages);

        // Nullify.
        memset(pSwapchain, 0, sizeof(VkxSwapchain));
    }
}
