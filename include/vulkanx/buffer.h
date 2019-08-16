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
#ifndef VULKANX_BUFFER_H
#define VULKANX_BUFFER_H

#include <vulkanx/memory.h>

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/**
 * @defgroup buffer Buffer
 *
 * `<vulkanx/buffer.h>`
 */
/**@{*/

/**
 * @brief Buffer.
 */
typedef struct VkxBuffer_
{
    /**
     * @brief Buffer.
     */
    VkBuffer buffer;

    /**
     * @brief Memory.
     */
    VkDeviceMemory memory;
}
VkxBuffer;

/**
 * @brief Create buffer.
 *
 * @param[in] physicalDevice
 * Physical device.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] pBufferCreateInfo
 * Buffer create info.
 *
 * @param[in] memoryPropertyFlags
 * Memory property flags.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[out] pBuffer
 * Buffer.
 *
 * @pre
 * - `physicalDevice` is valid
 * - `device` is valid
 * - `pBufferCreateInfo` is non-`NULL`
 * - `pBuffer` is non-`NULL`
 * - `pBuffer` is uninitialized
 */
VkResult vkxCreateBuffer(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            const VkBufferCreateInfo* pBufferCreateInfo,
            const VkMemoryPropertyFlags memoryPropertyFlags,
            const VkAllocationCallbacks* pAllocator,
            VkxBuffer* pBuffer)
                __attribute__((nonnull(3, 6)));

/**
 * @brief Destroy buffer.
 *
 * @param[in] device
 * Device.
 *
 * @param[inout] pBuffer
 * Buffer.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @note
 * If `pBuffer` is `NULL`, does nothing.
 *
 * @pre
 * If `pBuffer` is non-`NULL` with non-`NULL` members,
 * - `pBuffer` must have been initialized by `vkxCreateBuffer`
 * - `device` must have been passed to `vkxCreateBuffer`
 *
 * @post
 * If `pBuffer` is non-`NULL`,
 * - `pBuffer->buffer` is `VK_NULL_HANDLE`
 * - `pBuffer->memory` is `VK_NULL_HANDLE`
 */
void vkxDestroyBuffer(
            VkDevice device,
            VkxBuffer* pBuffer,
            const VkAllocationCallbacks* pAllocator);

/**
 * @brief Buffer group.
 */
typedef struct VkxBufferGroup_
{
    /**
     * @brief Buffer count.
     */
    uint32_t bufferCount;

    /**
     * @brief Buffers.
     */
    VkBuffer* pBuffers;

    /**
     * @brief Shared memory.
     */
    VkxSharedDeviceMemory sharedMemory;
}
VkxBufferGroup;

/**
 * @brief Create buffer group.
 *
 * @param[in] physicalDevice
 * Physical device.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] bufferCount
 * Buffer count.
 *
 * @param[in] pBufferCreateInfos
 * Buffer create infos.
 *
 * @param[in] pMemoryPropertyFlags
 * Memory property flags per buffer.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[out] pBufferGroup
 * Buffer group.
 *
 * @pre
 * - `physicalDevice` is valid
 * - `device` is valid
 * - `pBufferCreateInfos` points to `bufferCount` elements
 * - `pMemoryPropertyFlags` points to `bufferCount` elements
 * - `pBufferGroup` is non-`NULL`
 * - `pBufferGroup` is uninitialized
 */
VkResult vkxCreateBufferGroup(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            uint32_t bufferCount,
            const VkBufferCreateInfo* pBufferCreateInfos,
            const VkMemoryPropertyFlags* pMemoryPropertyFlags,
            const VkAllocationCallbacks* pAllocator,
            VkxBufferGroup* pBufferGroup)
                __attribute__((nonnull(7)));

/**
 * @brief Destroy buffer group.
 *
 * @param[in] device
 * Device.
 *
 * @param[inout] pBufferGroup
 * Buffer group.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @note
 * If `pBufferGroup` is `NULL`, does nothing.
 *
 * @pre
 * If `pBufferGroup` is non-`NULL` with non-`NULL` members,
 * - `pBufferGroup` must have been initialized by `vkxCreateBufferGroup`
 * - `device` must have been passed to `vkxCreateBufferGroup`
 *
 * @post
 * If `pBufferGroup` is non-`NULL`,
 * - `pBufferGroup->bufferCount` is `0`
 * - `pBufferGroup->pBuffers` is `NULL`
 * - `pBufferGroup->sharedMemory.uniqueMemoryCount` is `0`
 * - `pBufferGroup->sharedMemory.pUniqueMemories` is `NULL`
 * - `pBufferGroup->sharedMemory.memoryViewCount` is `0`
 * - `pBufferGroup->sharedMemory.pMemoryViews` is `NULL`
 */
void vkxDestroyBufferGroup(
            VkDevice device,
            VkxBufferGroup* pBufferGroup,
            const VkAllocationCallbacks* pAllocator);

/**
 * @brief Copy buffer.
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
 * @param[in] dstBuffer
 * Destination buffer.
 *
 * @param[in] bufferCopyRegionCount
 * Buffer copy region count.
 *
 * @param[in] pBufferCopyRegions
 * Buffer copy regions.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 */
VkResult vkxCopyBuffer(
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            VkBuffer srcBuffer,
            VkBuffer dstBuffer,
            uint32_t bufferCopyRegionCount,
            const VkBufferCopy* pBufferCopyRegions,
            const VkAllocationCallbacks* pAllocator);

/**
 * @brief Buffer region.
 */
typedef struct VkxBufferRegion_
{
    /**
     * @brief Offset in bytes.
     */
    VkDeviceSize offset;

    /**
     * @brief Size in bytes.
     */
    VkDeviceSize size;
}
VkxBufferRegion;

/**
 * @brief Get buffer data via temporary staging buffer.
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
 * @param[in] buffer
 * Buffer.
 *
 * @param[in] pBufferRegion
 * Buffer region.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[out] pData
 * Data.
 *
 * @note
 * Buffer must have been created to
 * support `VK_BUFFER_USAGE_TRANSFER_SRC_BIT`.
 */
VkResult vkxGetBufferData(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            VkBuffer buffer,
            const VkxBufferRegion* pBufferRegion,
            const VkAllocationCallbacks* pAllocator,
            void* pData)
                __attribute__((nonnull(6)));

/**
 * @brief Set buffer data via temporary staging buffer.
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
 * @param[in] buffer
 * Buffer.
 *
 * @param[in] pBufferRegion
 * Buffer region.
 *
 * @param[in] pData
 * Data.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @note
 * Buffer must have been created to
 * support `VK_BUFFER_USAGE_TRANSFER_DST_BIT`.
 */
VkResult vkxSetBufferData(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            VkBuffer buffer,
            const VkxBufferRegion* pBufferRegion,
            const void* pData,
            const VkAllocationCallbacks* pAllocator)
                __attribute__((nonnull(6)));

/**@}*/

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef VULKANX_BUFFER_H
