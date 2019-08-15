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
#pragma once
#ifndef VULKANX_IMAGE_H
#define VULKANX_IMAGE_H

#include <vulkanx/memory.h>

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/**
 * @defgroup image Image
 *
 * `<vulkanx/image.h>`
 */
/**@{*/

/**
 * @brief Image.
 */
typedef struct VkxImage_
{
    /**
     * @brief Image.
     */
    VkImage image;

    /**
     * @brief Memory.
     */
    VkDeviceMemory memory;
}
VkxImage;

/**
 * @brief Create image.
 *
 * @param[in] physicalDevice
 * Physical device.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] pImageCreateInfo
 * Image create info.
 *
 * @param[in] memoryPropertyFlags
 * Memory property flags.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[out] pImage
 * Image.
 *
 * @pre
 * - `physicalDevice` is valid
 * - `device` is valid
 * - `pImageCreateInfo` is non-`NULL`
 * - `pImage` is non-`NULL`
 * - `pImage` is uninitialized
 */
VkResult vkxCreateImage(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            const VkImageCreateInfo* pImageCreateInfo,
            const VkMemoryPropertyFlags memoryPropertyFlags,
            const VkAllocationCallbacks* pAllocator,
            VkxImage* pImage)
                __attribute__((nonnull(3, 6)));

/**
 * @brief Destroy image.
 *
 * @param[in] device
 * Device.
 *
 * @param[inout] pImage
 * Image.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @note
 * If `pImage` is `NULL`, does nothing.
 *
 * @pre
 * If `pImage` is non-`NULL` with non-`NULL` members,
 * - `pImage` must have been initialized by `vkxCreateImage`
 * - `device` must have been passed to `vkxCreateImage`
 *
 * @post
 * If `pImage` is non-`NULL`,
 * - `pImage->image` is `VK_NULL_HANDLE`
 * - `pImage->memory` is `VK_NULL_HANDLE`
 */
void vkxDestroyImage(
            VkDevice device, 
            VkxImage* pImage,
            const VkAllocationCallbacks* pAllocator);

/**
 * @brief Image group.
 */
typedef struct VkxImageGroup_
{
    /**
     * @brief Image count.
     */
    uint32_t imageCount;

    /**
     * @brief Images.
     */
    VkImage* pImages;

    /**
     * @brief Shared memory.
     */
    VkxSharedDeviceMemory sharedMemory;
}
VkxImageGroup;

/**
 * @brief Create image group.
 *
 * @param[in] physicalDevice
 * Physical device.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] imageCount
 * Image count.
 *
 * @param[in] pImageCreateInfos
 * Image create infos.
 *
 * @param[in] pMemoryPropertyFlags
 * Memory property flags per image.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[out] pImageGroup
 * Image group.
 *
 * @pre
 * - `physicalDevice` is valid
 * - `device` is valid
 * - `pImageCreateInfos` points to `imageCount` elements
 * - `pMemoryPropertyFlags` points to `imageCount` elements
 * - `pImageGroup` is non-`NULL`
 * - `pImageGroup` is uninitialized
 */
VkResult vkxCreateImageGroup(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            uint32_t imageCount,
            const VkImageCreateInfo* pImageCreateInfos,
            const VkMemoryPropertyFlags* pMemoryPropertyFlags,
            const VkAllocationCallbacks* pAllocator,
            VkxImageGroup* pImageGroup)
                __attribute__((nonnull(7)));

/**
 * @brief Destroy image group.
 *
 * @param[in] device
 * Device.
 *
 * @param[inout] pImageGroup
 * Image group.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @note
 * If `pImageGroup` is `NULL`, does nothing.
 *
 * @pre
 * If `pImageGroup` is non-`NULL` with non-`NULL` members,
 * - `pImageGroup` must have been initialized by `vkxCreateImageGroup`
 * - `device` must have been passed to `vkxCreateImageGroup`
 *
 * @post
 * If `pImageGroup` is non-`NULL`,
 * - `pImageGroup->imageCount` is `0`
 * - `pImageGroup->pImages` is `NULL`
 * - `pImageGroup->sharedMemory.uniqueMemoryCount` is `0`
 * - `pImageGroup->sharedMemory.pUniqueMemories` is `NULL`
 * - `pImageGroup->sharedMemory.memoryViewCount` is `0`
 * - `pImageGroup->sharedMemory.pMemoryViews` is `NULL`
 */
void vkxDestroyImageGroup(
            VkDevice device,
            VkxImageGroup* pImageGroup,
            const VkAllocationCallbacks* pAllocator);

/**
 * @brief Transition image layout.
 *
 * @param[in] commandBuffer
 * Command buffer.
 *
 * @param[in] image
 * Image.
 *
 * @param[in] oldLayout
 * Old image layout.
 *
 * @param[in] newLayout
 * New image layout.
 *
 * @param[in] subresourceRange
 * Subresource range.
 *
 * @param[in] srcStageMask
 * Stage mask.
 *
 * @param[in] dstStageMask
 * Stage mask.
 */
VkResult vkxCmdTransitionImageLayout(
            VkCommandBuffer commandBuffer,
            VkImage image,
            VkImageLayout oldLayout,
            VkImageLayout newLayout,
            VkImageSubresourceRange subresourceRange,
            VkPipelineStageFlags srcStageMask,
            VkPipelineStageFlags dstStageMask);

#if 0
/**
 * @brief Image data access info.
 */
typedef struct VkxImageDataAccessInfo_
{
    /**
     * @brief Image.
     */
    VkImage image;

    /**
     * @brief Old layout.
     */
    VkImageLayout oldLayout;

    /**
     * @brief New layout.
     */
    VkImageLayout newLayout;

    /**
     * @brief Subresource layers.
     */
    VkImageSubresourceLayers subresourceLayers;

    /**
     * @brief Offset.
     */
    VkOffset3D offset;

    /**
     * @brief Extent.
     */
    VkExtent3D extent;
}
VkxImageDataAccessInfo;

/**
 * @brief Get image data via temporary staging buffer.
 *
 * @param[in] physicalDevice
 * Physical device.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] queue
 * Queue.
 *
 * @param[in] commandPool
 * Command pool.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[in] pAccessInfo
 * Data access info.
 *
 * @param[in] dataSize
 * Data size in bytes.
 *
 * @param[out] pData
 * Data.
 *
 * @note
 * Image must have been created to 
 * support `VK_IMAGE_USAGE_TRANSFER_SRC_BIT`.
 */
VkResult vkxGetImageData(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            const VkAllocationCallbacks* pAllocator,
            const VkxImageDataAccessInfo* pAccessInfo,
            VkDeviceSize dataSize,
            void* pData)
                __attribute__((nonnull(5)));

/**
 * @brief Set image data via temporary staging buffer.
 *
 * @param[in] physicalDevice
 * Physical device.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] queue
 * Queue.
 *
 * @param[in] commandPool
 * Command pool.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 * 
 * @param[in] pAccessInfo
 * Data access info.
 *
 * @param[in] dataSize
 * Data size in bytes.
 *
 * @param[in] pData
 * Data.
 *
 * @note
 * Image must have been created to 
 * support `VK_IMAGE_USAGE_TRANSFER_DST_BIT`.
 */
VkResult vkxSetImageData(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            const VkAllocationCallbacks* pAllocator,
            const VkxImageDataAccessInfo* pAccessInfo,
            VkDeviceSize dataSize,
            const void* pData)
                __attribute__((nonnull(5)));
#endif

/**@}*/

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef VULKANX_IMAGE_H
