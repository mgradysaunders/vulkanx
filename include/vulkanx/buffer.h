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
 *
 * @post
 * - on success, `pBuffer` is properly initialized
 * - on failure, `pBuffer` is nullified
 */
VkResult vkxCreateBuffer(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            const VkBufferCreateInfo* pBufferCreateInfo,
            const VkMemoryPropertyFlags memoryPropertyFlags,
            const VkAllocationCallbacks* pAllocator,
            VkxBuffer* pBuffer);

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
 * @pre
 * - `device` is valid
 * - `device` was used to create `pBuffer`
 * - `pAllocator` was used to create `pBuffer`
 * - `pBuffer` was previously created by `vkxCreateBuffer`
 *
 * @post
 * - `pBuffer` is nullified
 *
 * @note
 * Does nothing if `pBuffer` is `NULL`.
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
 * - `pBufferCreateInfos` points to `bufferCount` values
 * - `pMemoryPropertyFlags` points to `bufferCount` values
 * - `pBufferGroup` is non-`NULL` 
 * - `pBufferGroup` is uninitialized
 *
 * @post
 * - on success, `pBufferGroup` is properly initialized
 * - on failure, `pBufferGroup` is nullified
 *
 * @note
 * If `bufferCount` is `0`,
 * `pBufferCreateInfos` may be `NULL`,
 * `pMemoryPropertyFlags` may be `NULL`,
 * `pBufferGroup` is nullified, and result is `VK_SUCCESS`.
 */
VkResult vkxCreateBufferGroup(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            uint32_t bufferCount,
            const VkBufferCreateInfo* pBufferCreateInfos,
            const VkMemoryPropertyFlags* pMemoryPropertyFlags,
            const VkAllocationCallbacks* pAllocator,
            VkxBufferGroup* pBufferGroup);

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
 * @pre
 * - `device` was used to create `pBufferGroup`
 * - `pAllocator` was used to create `pBufferGroup`
 * - `pBufferGroup` was previously created by `vkxCreateBufferGroup`
 *
 * @post
 * - `pBufferGroup` is nullified
 *
 * @note
 * Does nothing if `pBufferGroup` is `NULL`.
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
 * - `dstBuffer` supports `VK_BUFFER_USAGE_TRANSFER_DST_BIT`
 * - `pRegions` points to `regionCount` values
 * - `pRegions` is `NULL` only if `regionCount` is `0`, in which case
 * the implementation immediately returns `VK_SUCCESS`
 */
VkResult vkxCopyBuffer(
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            VkBuffer srcBuffer,
            VkBuffer dstBuffer,
            uint32_t regionCount,
            const VkBufferCopy* pRegions,
            const VkAllocationCallbacks* pAllocator);

/**
 * @brief Buffer data access.
 */
typedef struct VkxBufferDataAccess_
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
VkxBufferDataAccess;

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
 * @param[in] pBufferDataAccess
 * Buffer data access.
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
 * - `buffer` supports `VK_BUFFER_USAGE_TRANSFER_SRC_BIT`
 * - `pBufferDataAccess` is non-`NULL`
 * - `pData` points to `pBufferDataAccess->size` bytes
 * - `pData` is `NULL` only if `pBufferDataAccess->size` is `0`, in which case 
 * the implementation immediately returns `VK_SUCCESS`
 */
VkResult vkxGetBufferData(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            VkBuffer buffer,
            const VkxBufferDataAccess* pBufferDataAccess,
            const VkAllocationCallbacks* pAllocator,
            void* pData);

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
 * @param[in] pBufferDataAccess
 * Buffer data access.
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
 * - `buffer` supports `VK_BUFFER_USAGE_TRANSFER_DST_BIT`
 * - `pBufferDataAccess` is non-`NULL`
 * - `pData` points to `pBufferDataAccess->size` bytes
 * - `pData` is `NULL` only if `pBufferDataAccess->size` is `0`, in which case 
 * the implementation immediately returns `VK_SUCCESS`
 */
VkResult vkxSetBufferData(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            VkBuffer buffer,
            const VkxBufferDataAccess* pBufferDataAccess,
            const void* pData,
            const VkAllocationCallbacks* pAllocator);

/**@}*/

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef VULKANX_BUFFER_H
