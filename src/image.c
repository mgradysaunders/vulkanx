/* Copyright (c) 2019 M. Grady Saunders
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
/*+-+*/
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
        .srcQueueFamilyIndex = 0,
        .dstQueueFamilyIndex = 0,
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

#if 0
// Get image data.
void vkxGetImageData(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            const VkxImageDataAccessInfo* pAccessInfo,
            VkDeviceSize dataSize,
            void* pData)
{
    assert(pAccessInfo);
    assert(pData || dataSize == 0);
    if (dataSize == 0) {
        return;
    }

    // Create staging buffer.
    VkxBuffer dstBuffer;
    VkBufferCreateInfo dstBufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = (VkBufferCreateFlags)0,
        .size = dataSize,
        .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL
    };
    vkxCreateBuffer(
            physicalDevice,
            device,
            &dstBufferCreateInfo,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &dstBuffer);

    // Allocate command buffer.
    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
    (void)
    VKX_VERIFIED_CALL(
        vkAllocateCommandBuffers,
                device,
                &commandBufferAllocateInfo,
                &commandBuffer);

    // Begin command buffer.
    VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = NULL
    };
    (void)
    VKX_VERIFIED_CALL(
        vkBeginCommandBuffer,
                commandBuffer,
                &commandBufferBeginInfo);

    // Subresource range corresponding to subresource layers.
    VkImageSubresourceRange subresourceRange = {
        .aspectMask = pAccessInfo->subresourceLayers.aspectMask,
        .baseMipLevel = pAccessInfo->subresourceLayers.mipLevel,
        .baseArrayLayer = pAccessInfo->subresourceLayers.baseArrayLayer,
        .levelCount = 1,
        .layerCount = pAccessInfo->subresourceLayers.layerCount
    };

    // Transition from oldLayout to TRANSFER_SRC_OPTIMAL.
    vkxCmdTransitionImageLayout(
            commandBuffer,
            pAccessInfo->image,
            pAccessInfo->oldLayout,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            subresourceRange,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

    // Buffer image copy.
    VkBufferImageCopy bufferImageCopy = {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = pAccessInfo->subresourceLayers,
        .imageOffset = pAccessInfo->offset,
        .imageExtent = pAccessInfo->extent
    };
    vkCmdCopyImageToBuffer(
            commandBuffer,
            pAccessInfo->image,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            dstBuffer.buffer,
            1, &bufferImageCopy);

    // Transition from TRANSFER_SRC_OPTIMAL to newLayout.
    vkxCmdTransitionImageLayout(
            commandBuffer,
            pAccessInfo->image,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            pAccessInfo->newLayout,
            subresourceRange,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

    // End command buffer.
    vkEndCommandBuffer(commandBuffer);

    // Flush command buffer.
    vkxFlushCommandBuffers(device, queue, 1, &commandBuffer);

    // Free command buffer.
    vkFreeCommandBuffers(
            device,
            commandPool,
            1, &commandBuffer);

    // Read from staging buffer.
    void* pMappedMemory = NULL;
    (void)
    VKX_VERIFIED_CALL(
        vkMapMemory,
                device,
                dstBuffer.memory,
                (VkDeviceSize)0, dataSize,
                (VkMemoryMapFlags)0, 
                &pMappedMemory);
    memcpy(pData, pMappedMemory, dataSize);
    vkUnmapMemory(device, dstBuffer.memory);

    // Destroy staging buffer.
    vkxDestroyBuffer(device, &dstBuffer);
}

// Set image data.
void vkxSetImageData(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            const VkxImageDataAccessInfo* pAccessInfo,
            VkDeviceSize dataSize,
            const void* pData)
{
    assert(pAccessInfo);
    assert(pData || dataSize == 0);
    if (dataSize == 0) {
        return;
    }

    // Create staging buffer.
    VkxBuffer srcBuffer;
    VkBufferCreateInfo srcBufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = (VkBufferCreateFlags)0,
        .size = dataSize,
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL
    };
    vkxCreateBuffer(
            physicalDevice,
            device,
            &srcBufferCreateInfo,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &srcBuffer);

    // Write to staging buffer.
    void* pMappedMemory = NULL;
    (void)
    VKX_VERIFIED_CALL(
        vkMapMemory,
                device,
                srcBuffer.memory,
                (VkDeviceSize)0, dataSize,
                (VkMemoryMapFlags)0, 
                &pMappedMemory);
    memcpy(pMappedMemory, pData, dataSize);
    vkUnmapMemory(device, srcBuffer.memory);

    // Allocate command buffer.
    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
    (void)
    VKX_VERIFIED_CALL(
        vkAllocateCommandBuffers,
                device,
                &commandBufferAllocateInfo,
                &commandBuffer);

    // Begin command buffer.
    VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = NULL
    };
    (void)
    VKX_VERIFIED_CALL(
        vkBeginCommandBuffer,
                commandBuffer,
                &commandBufferBeginInfo);

    // Subresource range corresponding to subresource layers.
    VkImageSubresourceRange subresourceRange = {
        .aspectMask = pAccessInfo->subresourceLayers.aspectMask,
        .baseMipLevel = pAccessInfo->subresourceLayers.mipLevel,
        .baseArrayLayer = pAccessInfo->subresourceLayers.baseArrayLayer,
        .levelCount = 1,
        .layerCount = pAccessInfo->subresourceLayers.layerCount
    };

    // Transition from oldLayout to TRANSFER_DST_OPTIMAL.
    vkxCmdTransitionImageLayout(
            commandBuffer,
            pAccessInfo->image,
            pAccessInfo->oldLayout,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            subresourceRange,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

    // Buffer image copy.
    VkBufferImageCopy bufferImageCopy = {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = pAccessInfo->subresourceLayers,
        .imageOffset = pAccessInfo->offset,
        .imageExtent = pAccessInfo->extent
    };
    vkCmdCopyBufferToImage(
            commandBuffer,
            srcBuffer.buffer,
            pAccessInfo->image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &bufferImageCopy);

    // Transition from TRANSFER_DST_OPTIMAL to newLayout.
    vkxCmdTransitionImageLayout(
            commandBuffer,
            pAccessInfo->image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            pAccessInfo->newLayout,
            subresourceRange,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

    // End command buffer.
    vkEndCommandBuffer(commandBuffer);

    // Flush command buffer.
    vkxFlushCommandBuffers(device, queue, 1, &commandBuffer);

    // Free command buffer.
    vkFreeCommandBuffers(
            device,
            commandPool,
            1, &commandBuffer);

    // Destroy staging buffer.
    vkxDestroyBuffer(device, &srcBuffer);
}
#endif
