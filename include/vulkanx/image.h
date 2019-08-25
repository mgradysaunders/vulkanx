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
 *
 * @post
 * - on success, `pImage` is properly initialized
 * - on failure, `pImage` is nullified
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
 * @pre
 * - `device` was used to create `pImage`
 * - `pAllocator` was used to create `pImage`
 * - `pImage` was previously created by `vkxCreateImage`
 *
 * @post
 * - `pImage` is nullified
 *
 * @note
 * Does nothing if `pImage` is `NULL`.
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
 * Memory property flags.
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
 * - `pImageCreateInfos` points to `imageCount` values
 * - `pMemoryPropertyFlags` points to `imageCount` values
 * - `pImageGroup` is non-`NULL` 
 * - `pImageGroup` is uninitialized
 *
 * @post
 * - on success, `pImageGroup` is properly initialized
 * - on failure, `pImageGroup` is nullified
 *
 * @note
 * If `imageCount` is `0`, 
 * `pImageCreateInfos` may be `NULL`,
 * `pMemoryPropertyFlags` may be `NULL`, 
 * `pImageGroup` is nullified, and result is `VK_SUCCESS`.
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
 * @pre
 * - `device` was used to create `pImageGroup`
 * - `pAllocator` was used to create `pImageGroup`
 * - `pImageGroup` was previously created by `vkxCreateImageGroup`
 *
 * @post
 * - `pImageGroup` is nullified
 *
 * @note
 * Does nothing if `pImageGroup` is `NULL`.
 */
void vkxDestroyImageGroup(
            VkDevice device,
            VkxImageGroup* pImageGroup,
            const VkAllocationCallbacks* pAllocator);


/**
 * @brief Create default image view.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] pImageCreateInfo
 * Image create info.
 *
 * @param[in] image
 * Image.
 *
 * @param[in] imageAspectMask
 * Image aspect mask.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[out] pImageView
 * Image view.
 */
VkResult vkxCreateDefaultImageView(
            VkDevice device,
            const VkImageCreateInfo* pImageCreateInfo,
            VkImage image,
            VkImageAspectFlags imageAspectMask,
            const VkAllocationCallbacks* pAllocator,
            VkImageView* pImageView)
                __attribute__((nonnull(2, 6)));

/**
 * @brief Transition image layout command.
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
 *
 * @pre
 * - `commandBuffer` is valid 
 * - `commandBuffer` is in the recording state
 * - `image` is valid
 */
VkResult vkxCmdTransitionImageLayout(
            VkCommandBuffer commandBuffer,
            VkImage image,
            VkImageLayout oldLayout,
            VkImageLayout newLayout,
            VkImageSubresourceRange subresourceRange,
            VkPipelineStageFlags srcStageMask,
            VkPipelineStageFlags dstStageMask);

/**
 * @brief Transition image layout.
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
 * @param[in] image
 * Image.
 *
 * @param[in] oldLayout
 * Old layout.
 *
 * @param[in] newLayout
 * New layout.
 *
 * @param[in] subresourceRange
 * Subresource range.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @pre
 * - all Vulkan handles are valid
 * - `queue` is compatible with `commandPool`
 */
VkResult vkxTransitionImageLayout(
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            VkImage image,
            VkImageLayout oldLayout,
            VkImageLayout newLayout,
            VkImageSubresourceRange subresourceRange,
            const VkAllocationCallbacks* pAllocator);

/**
 * @brief Copy image to buffer.
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
 * @param[in] srcImage
 * Source image.
 *
 * @param[in] srcImageLayout
 * Source image layout.
 *
 * @param[in] dstBuffer
 * Destination buffer.
 *
 * @param[in] regionCount
 * Region count.
 *
 * @param[in] pRegions
 * Regions.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @pre
 * - all Vulkan handles are valid
 * - `queue` is compatible with `commandPool`
 * - `srcImage` supports `VK_IMAGE_USAGE_TRANSFER_SRC_BIT`
 * - `srcImageLayout` is 
 * `VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL`,
 * `VK_IMAGE_LAYOUT_GENERAL`, or `VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR`
 * - `dstBuffer` supports `VK_BUFFER_USAGE_TRANSFER_SRC_BIT`
 * - `pRegions` points to `regionCount` values
 * - `pRegions` is `NULL` only if `regionCount` is `0`, in which case
 * the implementation immediately returns `VK_SUCCESS`
 */
VkResult vkxCopyImageToBuffer(
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            VkImage srcImage,
            VkImageLayout srcImageLayout,
            VkBuffer dstBuffer,
            uint32_t regionCount,
            const VkBufferImageCopy* pRegions,
            const VkAllocationCallbacks* pAllocator);

/**
 * @brief Copy buffer to image.
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
 * @param[in] srcBuffer
 * Source buffer.
 *
 * @param[in] dstImage
 * Destination image.
 *
 * @param[in] dstImageLayout
 * Destination image layout.
 *
 * @param[in] regionCount
 * Region count.
 *
 * @param[in] pRegions
 * Regions.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @pre
 * - all Vulkan handles are valid
 * - `queue` is compatible with `commandPool`
 * - `srcBuffer` supports `VK_BUFFER_USAGE_TRANSFER_SRC_BIT`
 * - `dstImage` supports `VK_IMAGE_USAGE_TRANSFER_DST_BIT`
 * - `dstImageLayout` is 
 * `VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL`,
 * `VK_IMAGE_LAYOUT_GENERAL`, or `VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR`
 * - `pRegions` points to `regionCount` values
 * - `pRegions` is `NULL` only if `regionCount` is `0`, in which case
 * the implementation immediately returns `VK_SUCCESS`
 */
VkResult vkxCopyBufferToImage(
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            VkBuffer srcBuffer,
            VkImage dstImage,
            VkImageLayout dstImageLayout,
            uint32_t regionCount,
            const VkBufferImageCopy* pRegions,
            const VkAllocationCallbacks* pAllocator);

/**
 * @brief Image data access.
 */
typedef struct VkxImageDataAccess_
{
    /**
     * @brief Layout.
     */
    VkImageLayout layout;

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

    /**
     * @brief Size in bytes.
     */
    VkDeviceSize size;
}
VkxImageDataAccess;

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
 * @param[in] image
 * Image.
 * 
 * @param[in] pImageDataAccess
 * Image data access.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[out] pData
 * Data.
 *
 * @pre
 * - all Vulkan handles are valid
 * - `queue` is compatible with `commandPool`
 * - `image` supports `VK_IMAGE_USAGE_TRANSFER_SRC_BIT`
 * - `pImageDataAccess` is non-`NULL`
 * - `pImageDataAccess->layout` is
 * `VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL`,
 * `VK_IMAGE_LAYOUT_GENERAL`, or `VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR`
 * - `pData` points to `pImageDataAccess->size` bytes
 * - `pData` is `NULL` only if `pImageDataAccess->size` is `0`, in which case
 * the implementation immediately returns `VK_SUCCESS`
 */
VkResult vkxGetImageData(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            VkImage image,
            const VkxImageDataAccess* pImageDataAccess,
            const VkAllocationCallbacks* pAllocator,
            void* pData)
                __attribute__((nonnull(6)));

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
 * @param[in] image
 * Image.
 * 
 * @param[in] pImageDataAccess
 * Image data access.
 *
 * @param[in] pData
 * Data.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @pre
 * - all Vulkan handles are valid
 * - `queue` is compatible with `commandPool`
 * - `image` supports `VK_IMAGE_USAGE_TRANSFER_DST_BIT`
 * - `pImageDataAccess` is non-`NULL`
 * - `pImageDataAccess->layout` is
 * `VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL`,
 * `VK_IMAGE_LAYOUT_GENERAL`, or `VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR`
 * - `pData` points to `pImageDataAccess->size` bytes
 * - `pData` is `NULL` only if `pImageDataAccess->size` is `0`, in which case 
 * the implementation immediately returns `VK_SUCCESS`
 */
VkResult vkxSetImageData(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            VkImage image,
            const VkxImageDataAccess* pImageDataAccess,
            const void* pData,
            const VkAllocationCallbacks* pAllocator)
                __attribute__((nonnull(6)));

/**@}*/

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef VULKANX_IMAGE_H
