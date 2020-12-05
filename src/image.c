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
#include <vulkanx/buffer.h>
#include <vulkanx/image.h>

// Create image.
VkResult vkxCreateImage(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            const VkImageCreateInfo* pImageCreateInfo,
            const VkMemoryPropertyFlags memoryPropertyFlags,
            const VkAllocationCallbacks* pAllocator,
            VkxImage* pImage)
{
    assert(pImageCreateInfo);
    assert(pImage);

    // Nullify.
    pImage->image = VK_NULL_HANDLE;
    pImage->memory = VK_NULL_HANDLE;

    {
        // Create image.
        VkResult result = 
            vkCreateImage(
                    device,
                    pImageCreateInfo, pAllocator,
                    &pImage->image);
        if (VKX_IS_ERROR(result)) {
            pImage->image = VK_NULL_HANDLE;
            return result;
        }
    }

    {
        // Get memory requirements.
        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(
                device,
                pImage->image,
                &memoryRequirements);

        // Find memory type index.
        uint32_t memoryTypeIndex = 
            vkxFindMemoryTypeIndex(
                    physicalDevice,
                    memoryPropertyFlags,
                    memoryRequirements.memoryTypeBits);
        if (memoryTypeIndex == UINT32_MAX) {
            // Destroy image.
            vkxDestroyImage(device, pImage, pAllocator);
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        // Memory allocate info.
        VkMemoryAllocateInfo memoryAllocateInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = NULL,
            .allocationSize = memoryRequirements.size,
            .memoryTypeIndex = memoryTypeIndex
        };

        // Allocate memory.
        VkResult result = 
            vkAllocateMemory(
                    device,
                    &memoryAllocateInfo, pAllocator,
                    &pImage->memory);
        if (VKX_IS_ERROR(result)) {
            // Destroy image.
            pImage->memory = VK_NULL_HANDLE;
            vkxDestroyImage(device, pImage, pAllocator);
            return result;
        }
    }

    // Bind memory.
    VkResult result = 
        vkBindImageMemory(
                device,
                pImage->image,
                pImage->memory,
                0);
    if (VKX_IS_ERROR(result)) {
        // Destroy image.
        vkxDestroyImage(device, pImage, pAllocator);
        // Fall through.
    }
    return result;
}

// Destroy image.
void vkxDestroyImage(
            VkDevice device, 
            VkxImage* pImage,
            const VkAllocationCallbacks* pAllocator)
{
    if (pImage) {
        // Destroy image.
        vkDestroyImage(
                device, 
                pImage->image,
                pAllocator);

        // Free memory.
        vkFreeMemory(
                device, 
                pImage->memory, 
                pAllocator);

        // Nullify.
        pImage->image = VK_NULL_HANDLE;
        pImage->memory = VK_NULL_HANDLE;
    }
}

// Create image group.
VkResult vkxCreateImageGroup(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            uint32_t imageCount,
            const VkImageCreateInfo* pImageCreateInfos,
            const VkMemoryPropertyFlags* pMemoryPropertyFlags,
            const VkAllocationCallbacks* pAllocator,
            VkxImageGroup* pImageGroup)
{
    assert(pImageGroup);
    memset(pImageGroup, 0, sizeof(VkxImageGroup));
    if (imageCount == 0) {
        return VK_SUCCESS;
    }

    // Sanity check.
    assert(pImageCreateInfos);
    assert(pMemoryPropertyFlags);

    // Allocate images.
    pImageGroup->imageCount = imageCount;
    pImageGroup->pImages = 
        (VkImage*)malloc(sizeof(VkImage) * imageCount);
    VkImage* pImages = pImageGroup->pImages;
    for (uint32_t imageIndex = 0;
                  imageIndex < imageCount; imageIndex++) {
        pImages[imageIndex] = VK_NULL_HANDLE;
    }

    // Allocate memory requirements.
    VkMemoryRequirements* pMemoryRequirements = 
        (VkMemoryRequirements*)VKX_LOCAL_MALLOC(
                sizeof(VkMemoryRequirements) * imageCount);

    // Iterate images.
    for (uint32_t imageIndex = 0; 
                  imageIndex < imageCount;
                  imageIndex++) {
        // Create image.
        VkResult result = 
            vkCreateImage(
                    device,
                    &pImageCreateInfos[imageIndex], pAllocator,
                    &pImages[imageIndex]);
        if (VKX_IS_ERROR(result)) {
            // Free memory requirements.
            VKX_LOCAL_FREE(pMemoryRequirements);
            // Destroy image group.
            pImages[imageIndex] = VK_NULL_HANDLE;
            vkxDestroyImageGroup(device, pImageGroup, pAllocator);
            return result;
        }

        // Get image memory requirements.
        vkGetImageMemoryRequirements(
                device, 
                pImages[imageIndex],
                &pMemoryRequirements[imageIndex]);
    }

    {
        // Allocate shared memory.
        VkResult result = 
            vkxAllocateSharedMemory(
                    physicalDevice,
                    device,
                    imageCount,
                    pMemoryRequirements,
                    pMemoryPropertyFlags,
                    pAllocator,
                    &pImageGroup->sharedMemory);
        if (VKX_IS_ERROR(result)) {
            // Free memory requirements.
            VKX_LOCAL_FREE(pMemoryRequirements);
            // Destroy image group.
            vkxDestroyImageGroup(device, pImageGroup, pAllocator);
            return result;
        }
    }

    // Bind memory.
    VkxDeviceMemoryView* pMemoryViews = 
        pImageGroup->sharedMemory.pMemoryViews;
    for (uint32_t imageIndex = 0;
                  imageIndex < imageCount;
                  imageIndex++) {
        VkResult result = 
            vkBindImageMemory(
                    device,
                    pImages[imageIndex],
                    pMemoryViews[imageIndex].memory,
                    pMemoryViews[imageIndex].offset);
        if (VKX_IS_ERROR(result)) {
            // Free memory requirements.
            VKX_LOCAL_FREE(pMemoryRequirements);
            // Destroy image group.
            vkxDestroyImageGroup(device, pImageGroup, pAllocator);
            return result;
        }
    }

    // Free memory requirements.
    VKX_LOCAL_FREE(pMemoryRequirements);

    return VK_SUCCESS;
}

// Destroy image group.
void vkxDestroyImageGroup(
            VkDevice device,
            VkxImageGroup* pImageGroup,
            const VkAllocationCallbacks* pAllocator)
{
    if (pImageGroup) {
        for (uint32_t imageIndex = 0;
                      imageIndex < pImageGroup->imageCount;
                      imageIndex++) {
            // Destroy image.
            vkDestroyImage(
                    device,
                    pImageGroup->pImages[imageIndex],
                    pAllocator);
        }
        // Free images.
        free(pImageGroup->pImages);

        // Nullify.
        pImageGroup->pImages = NULL;
        pImageGroup->imageCount = 0;

        // Delegate.
        vkxFreeSharedMemory(device, &pImageGroup->sharedMemory, pAllocator);
    }
}

// Create default image view.
VkResult vkxCreateDefaultImageView(
            VkDevice device,
            VkImage image,
            const VkImageCreateInfo* pImageCreateInfo, 
            const VkAllocationCallbacks* pAllocator,
            VkImageView* pImageView)
{
    assert(pImageCreateInfo);
    assert(pImageView);

    // Sanity check.
    // At the time of writing, the Vulkan specification defines the
    // 1D, 2D, and 3D enumerations of VkImageType and VkImageViewType as 0, 1,
    // and 2 respectively.
    _Static_assert(
            (int)VK_IMAGE_TYPE_1D == (int)VK_IMAGE_VIEW_TYPE_1D &&
            (int)VK_IMAGE_TYPE_2D == (int)VK_IMAGE_VIEW_TYPE_2D &&
            (int)VK_IMAGE_TYPE_3D == (int)VK_IMAGE_VIEW_TYPE_3D,
            "Invalid assumption");

    // View type.
    VkImageViewType viewType = (VkImageViewType)pImageCreateInfo->imageType;
    // Is cube compatible with multiple of 6 array layers?
    if ((pImageCreateInfo->flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT) &&
         pImageCreateInfo->arrayLayers % 6 == 0) {
        viewType = pImageCreateInfo->arrayLayers > 6 
            ? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY 
            : VK_IMAGE_VIEW_TYPE_CUBE;
    }
    // Is 1D image with more than 1 array layer?
    else if (pImageCreateInfo->imageType == VK_IMAGE_TYPE_1D &&
             pImageCreateInfo->arrayLayers > 1) {
        viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    }
    // Is 2D image with more than 1 array layer?
    else if (pImageCreateInfo->imageType == VK_IMAGE_TYPE_2D &&
             pImageCreateInfo->arrayLayers > 1) {
        viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    }

    // Deduce image aspect mask.
    VkImageAspectFlags imageAspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    // Is depth format?
    if (pImageCreateInfo->format == VK_FORMAT_D16_UNORM ||
        pImageCreateInfo->format == VK_FORMAT_X8_D24_UNORM_PACK32 ||
        pImageCreateInfo->format == VK_FORMAT_D32_SFLOAT) {
        imageAspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    // Is stencil format?
    else if (pImageCreateInfo->format == VK_FORMAT_S8_UINT) {
        imageAspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    // Is depth/stencil format?
    else if (pImageCreateInfo->format == VK_FORMAT_D16_UNORM_S8_UINT ||
             pImageCreateInfo->format == VK_FORMAT_D24_UNORM_S8_UINT ||
             pImageCreateInfo->format == VK_FORMAT_D32_SFLOAT_S8_UINT) {
        imageAspectMask = 
            VK_IMAGE_ASPECT_DEPTH_BIT | 
            VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    VkImageViewCreateInfo imageViewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .image = image,
        .viewType = viewType,
        .format = pImageCreateInfo->format,
        .components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY
        },
        .subresourceRange = {
            .aspectMask = imageAspectMask,
            .baseMipLevel = 0, .levelCount = pImageCreateInfo->mipLevels,
            .baseArrayLayer = 0, .layerCount = pImageCreateInfo->arrayLayers
        }
    };

    return vkCreateImageView(
                device,
                &imageViewCreateInfo, pAllocator,
                pImageView);
}

// Create default image views.
VkResult vkxCreateDefaultImageViews(
            VkDevice device,
            uint32_t imageCount,
            const VkImage* pImages,
            const VkImageCreateInfo* pImageCreateInfos,
            const VkAllocationCallbacks* pAllocator,
            VkImageView* pImageViews)
{
    if (imageCount == 0) {
        return VK_SUCCESS;
    }

    assert(pImages && pImageCreateInfos);

    for (uint32_t imageIndex = 0;
                  imageIndex < imageCount;
                  imageIndex++) {
        // Nullify.
        pImageViews[imageIndex] = VK_NULL_HANDLE;
    }

    VkResult result = VK_SUCCESS;
    for (uint32_t imageIndex = 0;
                  imageIndex < imageCount;
                  imageIndex++) {
        // Create default image view.
        result = 
            vkxCreateDefaultImageView(
                    device,
                    pImages[imageIndex],
                    &pImageCreateInfos[imageIndex], pAllocator,
                    &pImageViews[imageIndex]);
        if (VKX_IS_ERROR(result)) {
            pImageViews[imageIndex] = VK_NULL_HANDLE;
            break;
        }
    }

    if (VKX_IS_ERROR(result)) {
        for (uint32_t imageIndex = 0;
                      imageIndex < imageCount;
                      imageIndex++) {
            // Destroy.
            vkDestroyImageView(
                    device,
                    pImageViews[imageIndex],
                    pAllocator);
            // Nullify.
            pImageViews[imageIndex] = VK_NULL_HANDLE;
        }
    }
    return result;
}

// Transition image layout.
VkResult vkxCmdTransitionImageLayout(
            VkCommandBuffer commandBuffer,
            VkImage image,
            VkImageLayout oldLayout,
            VkImageLayout newLayout,
            VkImageSubresourceRange subresourceRange,
            VkPipelineStageFlags srcStageMask,
            VkPipelineStageFlags dstStageMask)
{
    // Image memory barrier.
    VkImageMemoryBarrier memoryBarrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = NULL,
        .srcAccessMask = 0, // Uninitialized.
        .dstAccessMask = 0, // Uninitialized.
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = subresourceRange
    };

    // Initialize srcAccessMask.
    switch (oldLayout) {
        // Image undefined/ignored.
        case VK_IMAGE_LAYOUT_UNDEFINED:
            break;
        
        // Image preinitialized.
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            memoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;

        // Image used as color attachment.
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            memoryBarrier.srcAccessMask =
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        // Image used as depth/stencil attachment.
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            memoryBarrier.srcAccessMask = 
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        // Image used as transfer source.
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        // Image used as transfer destination.
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        // Image read by shader.
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            memoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;

        default:
            return VK_ERROR_INITIALIZATION_FAILED;
            break;
    }

    // Initialize dstAccessMask.
    switch (newLayout) {

        // Image used as transfer destination.
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        // Image used as transfer source.
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        // Image used as color attachment.
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            memoryBarrier.dstAccessMask = 
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        // Image used as depth/stencil attachment.
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            memoryBarrier.dstAccessMask = 
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        // Image read by shader.
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            if (memoryBarrier.srcAccessMask == (VkAccessFlags)0) {
                memoryBarrier.srcAccessMask = 
                            VK_ACCESS_HOST_WRITE_BIT |
                            VK_ACCESS_TRANSFER_WRITE_BIT;
            }
            memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;

        default:
            return VK_ERROR_INITIALIZATION_FAILED;
            break;
    }

    // Record barrier.
    vkCmdPipelineBarrier(
            commandBuffer,
            srcStageMask,
            dstStageMask,
            0,
            0, NULL,
            0, NULL,
            1, &memoryBarrier);
    return VK_SUCCESS;
}

// Transition image layout.
VkResult vkxTransitionImageLayout(
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            VkImage image,
            VkImageLayout oldLayout,
            VkImageLayout newLayout,
            VkImageSubresourceRange subresourceRange,
            const VkAllocationCallbacks* pAllocator)
{
    // Command buffer allocate info.
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = commandPool,
        .commandBufferCount = 1
    };

    // Command buffer begin info.
    VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = NULL
    };

    // Command buffer.
    VkCommandBuffer commandBuffer;
    {
        // Allocate and begin command buffer.
        VkResult result = 
            vkxAllocateAndBeginCommandBuffers(
                    device,
                    &commandBufferAllocateInfo,
                    &commandBufferBeginInfo,
                    &commandBuffer);
        // Allocate and begin command buffer error?
        if (VKX_IS_ERROR(result)) {
            return result;
        }
    }

    // Transition image layout.
    VkResult result =
        vkxCmdTransitionImageLayout(
                commandBuffer,
                image,
                oldLayout,
                newLayout,
                subresourceRange,
                VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

    // Transition image layout error?
    if (VKX_IS_ERROR(result)) {
        // End command buffer.
        vkEndCommandBuffer(commandBuffer);
        // Free command buffer.
        vkFreeCommandBuffers(
                device,
                commandPool,
                1, &commandBuffer);
        return result;
    }


    // End, flush, and free command buffer.
    return vkxEndFlushAndFreeCommandBuffers(
                device,
                queue,
                commandPool,
                1, &commandBuffer,
                pAllocator);
}

// Copy image to buffer.
VkResult vkxCopyImageToBuffer(
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            VkImage srcImage,
            VkImageLayout srcImageLayout,
            VkBuffer dstBuffer,
            uint32_t regionCount,
            const VkBufferImageCopy* pRegions,
            const VkAllocationCallbacks* pAllocator)
{
    assert(
        srcImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ||
        srcImageLayout == VK_IMAGE_LAYOUT_GENERAL ||
        srcImageLayout == VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR);
    if (regionCount == 0) {
        return VK_SUCCESS;
    }

    // Sanity check.
    assert(pRegions);

    // Command buffer allocate info.
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = commandPool,
        .commandBufferCount = 1
    };

    // Command buffer begin info.
    VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = NULL
    };

    // Command buffer.
    VkCommandBuffer commandBuffer;
    {
        // Allocate and begin command buffer.
        VkResult result = 
            vkxAllocateAndBeginCommandBuffers(
                    device,
                    &commandBufferAllocateInfo,
                    &commandBufferBeginInfo,
                    &commandBuffer);
        // Allocate and begin command buffer error?
        if (VKX_IS_ERROR(result)) {
            return result;
        }
    }

    // Copy image to buffer.
    vkCmdCopyImageToBuffer(
            commandBuffer,
            srcImage,
            srcImageLayout,
            dstBuffer,
            regionCount,
            pRegions);

    // End, flush, and free command buffer.
    return vkxEndFlushAndFreeCommandBuffers(
                device,
                queue,
                commandPool,
                1, &commandBuffer,
                pAllocator);
}

// Copy buffer to image.
VkResult vkxCopyBufferToImage(
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            VkBuffer srcBuffer,
            VkImage dstImage,
            VkImageLayout dstImageLayout,
            uint32_t regionCount,
            const VkBufferImageCopy* pRegions,
            const VkAllocationCallbacks* pAllocator)
{
    assert(
        dstImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ||
        dstImageLayout == VK_IMAGE_LAYOUT_GENERAL ||
        dstImageLayout == VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR);
    if (regionCount == 0) {
        return VK_SUCCESS;
    }

    // Sanity check.
    assert(pRegions);

    // Command buffer allocate info.
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = commandPool,
        .commandBufferCount = 1
    };

    // Command buffer begin info.
    VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = NULL
    };

    // Command buffer.
    VkCommandBuffer commandBuffer;
    {
        // Allocate and begin command buffer.
        VkResult result = 
            vkxAllocateAndBeginCommandBuffers(
                    device,
                    &commandBufferAllocateInfo,
                    &commandBufferBeginInfo,
                    &commandBuffer);
        // Allocate and begin command buffer error?
        if (VKX_IS_ERROR(result)) {
            return result;
        }
    }

    // Copy buffer to image.
    vkCmdCopyBufferToImage(
            commandBuffer,
            srcBuffer,
            dstImage,
            dstImageLayout,
            regionCount,
            pRegions);

    // End, flush, and free command buffer.
    return vkxEndFlushAndFreeCommandBuffers(
                device,
                queue,
                commandPool,
                1, &commandBuffer,
                pAllocator);
}

// Get image data.
VkResult vkxGetImageData(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            VkImage image,
            const VkxImageDataAccess* pImageDataAccess,
            const VkAllocationCallbacks* pAllocator,
            void* pData)
{
    assert(pImageDataAccess);
    assert(
        pImageDataAccess->layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ||
        pImageDataAccess->layout == VK_IMAGE_LAYOUT_GENERAL ||
        pImageDataAccess->layout == VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR);
    assert(pData || pImageDataAccess->size == 0);
    if (pImageDataAccess->size == 0) {
        return VK_SUCCESS;
    }

    // Staging buffer.
    VkxBuffer stagingBuffer;
    {
        // Staging buffer create info.
        VkBufferCreateInfo bufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .size = pImageDataAccess->size,
            .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = NULL
        };
        // Create staging buffer.
        VkResult result = 
            vkxCreateBuffer(
                    physicalDevice,
                    device,
                    &bufferCreateInfo,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    pAllocator,
                    &stagingBuffer);
        // Create staging buffer error?
        if (VKX_IS_ERROR(result)) {
            return result;
        }
    }

    {
        // Copy image to buffer.
        VkBufferImageCopy region = {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = pImageDataAccess->subresourceLayers,
            .imageOffset = pImageDataAccess->offset,
            .imageExtent = pImageDataAccess->extent
        };
        VkResult result = 
            vkxCopyImageToBuffer(
                    device,
                    queue,
                    commandPool,
                    image, pImageDataAccess->layout,
                    stagingBuffer.buffer,
                    1, &region,
                    pAllocator);
        // Copy image to buffer error?
        if (VKX_IS_ERROR(result)) {
            // Destroy staging buffer.
            vkxDestroyBuffer(device, &stagingBuffer, pAllocator);
            return result;
        }
    }

    // Map staging buffer data.
    void* pStagingBufferData = NULL;
    VkResult result = 
        vkMapMemory(
                device,
                stagingBuffer.memory,
                0, pImageDataAccess->size,
                0, &pStagingBufferData);
    // Map staging buffer data ok?
    if (VKX_IS_OK(result)) {
        // Read staging buffer data.
        memcpy(pData, pStagingBufferData, pImageDataAccess->size);
        // Unmap staging buffer data.
        vkUnmapMemory(device, stagingBuffer.memory);
    }

    // Destroy staging buffer.
    vkxDestroyBuffer(device, &stagingBuffer, pAllocator);

    return result;
}

// Set image data.
VkResult vkxSetImageData(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            VkImage image,
            const VkxImageDataAccess* pImageDataAccess,
            const void* pData,
            const VkAllocationCallbacks* pAllocator)
{
    assert(pImageDataAccess);
    assert(
        pImageDataAccess->layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ||
        pImageDataAccess->layout == VK_IMAGE_LAYOUT_GENERAL ||
        pImageDataAccess->layout == VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR);
    assert(pData || pImageDataAccess->size == 0);
    if (pImageDataAccess->size == 0) {
        return VK_SUCCESS;
    }

    // Staging buffer.
    VkxBuffer stagingBuffer;
    {
        // Staging buffer create info.
        VkBufferCreateInfo bufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .size = pImageDataAccess->size,
            .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = NULL
        };
        // Create staging buffer.
        VkResult result = 
            vkxCreateBuffer(
                    physicalDevice,
                    device,
                    &bufferCreateInfo,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    pAllocator,
                    &stagingBuffer);
        // Create staging buffer error?
        if (VKX_IS_ERROR(result)) {
            return result;
        }
    }

    {
        // Map staging buffer data.
        void* pStagingBufferData = NULL;
        VkResult result = 
            vkMapMemory(
                    device,
                    stagingBuffer.memory,
                    0, pImageDataAccess->size,
                    0, &pStagingBufferData);
        // Map staging buffer data error?
        if (VKX_IS_ERROR(result)) {
            // Destroy staging buffer.
            vkxDestroyBuffer(device, &stagingBuffer, pAllocator);
            return result;
        }

        // Write staging buffer data.
        memcpy(pStagingBufferData, pData, pImageDataAccess->size);

        // Unmap staging buffer data.
        vkUnmapMemory(device, stagingBuffer.memory);
    }

    // Copy buffer to image.
    VkBufferImageCopy region = {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = pImageDataAccess->subresourceLayers,
        .imageOffset = pImageDataAccess->offset,
        .imageExtent = pImageDataAccess->extent
    };
    VkResult result = 
        vkxCopyBufferToImage(
                device,
                queue,
                commandPool,
                stagingBuffer.buffer,
                image, pImageDataAccess->layout,
                1, &region,
                pAllocator);

    // Destroy staging buffer.
    vkxDestroyBuffer(device, &stagingBuffer, pAllocator);

    return result;
}
