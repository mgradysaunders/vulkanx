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
#ifndef VULKANX_COMMAND_BUFFER_H
#define VULKANX_COMMAND_BUFFER_H

#include <vulkan/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/**
 * @defgroup command_buffer Command buffer
 *
 * `<vulkanx/command_buffer.h>`
 */
/**@{*/

/**
 * @brief Create fences.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] fenceCount
 * Fence count.
 *
 * @param[in] fenceSignaled
 * Fence signaled?
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[out] pFences
 * Fences.
 */
VkResult vkxCreateFences(
                VkDevice device,
                uint32_t fenceCount,
                VkBool32 fenceSignaled,
                const VkAllocationCallbacks* pAllocator,
                VkFence* pFences);

/**
 * @brief Destroy fences.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] fenceCount
 * Fence count.
 *
 * @param[inout] pFences
 * Fences.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 */
void vkxDestroyFences(
            VkDevice device,
            uint32_t fenceCount,
            VkFence* pFences,
            const VkAllocationCallbacks* pAllocator);

/**
 * @brief Create semaphores.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] semaphoreCount
 * Semaphore count.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[out] pSemaphores
 * Semaphores.
 */
VkResult vkxCreateSemaphores(
                VkDevice device,
                uint32_t semaphoreCount,
                const VkAllocationCallbacks* pAllocator,
                VkSemaphore* pSemaphores);

/**
 * @brief Destroy semaphores.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] semaphoreCount
 * Semaphore count.
 *
 * @param[inout] pSemaphores
 * Semaphores.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 */
void vkxDestroySemaphores(
            VkDevice device,
            uint32_t semaphoreCount,
            VkSemaphore* pSemaphores,
            const VkAllocationCallbacks* pAllocator);

/**
 * @brief Allocate and begin command buffers.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] pAllocateInfo
 * Allocate info.
 *
 * @param[in] pBeginInfos
 * Begin info per command buffer.
 *
 * @param[out] pCommandBuffers
 * Command buffers.
 */
VkResult vkxAllocateAndBeginCommandBuffers(
            VkDevice device,
            const VkCommandBufferAllocateInfo* pAllocateInfo,
            const VkCommandBufferBeginInfo* pBeginInfos,
            VkCommandBuffer* pCommandBuffers);

/**
 * @brief Flush command buffers.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] queue
 * Queue.
 *
 * @param[in] commandBufferCount
 * Command buffer count.
 *
 * @param[in] pCommandBuffers
 * Command buffers.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 */
VkResult vkxFlushCommandBuffers(
            VkDevice device,
            VkQueue queue,
            uint32_t commandBufferCount,
            const VkCommandBuffer* pCommandBuffers,
            const VkAllocationCallbacks* pAllocator);

/**
 * @brief End, flush, and free command buffers.
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
 * @param[in] commandBufferCount
 * Command buffer count.
 *
 * @param[in] pCommandBuffers
 * Command buffers.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 */
VkResult vkxEndFlushAndFreeCommandBuffers(
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            uint32_t commandBufferCount,
            const VkCommandBuffer* pCommandBuffers,
            const VkAllocationCallbacks* pAllocator);

/**
 * @brief Begin command buffer.
 *
 * @param[in] commandBuffer
 * Command buffer.
 *
 * @param[in] flags
 * Usage flags.
 *
 * @param[in] pInheritanceInfo
 * _Optional_. Inheritance info.
 *
 * @note
 * This does not do anything fancy, it just bypasses the need to initialize a
 * temporary `VkCommandBufferBeginInfo` for `vkBeginCommandBuffer`.
 */
void vkxBeginCommandBuffer(
            VkCommandBuffer commandBuffer,
            VkCommandBufferUsageFlags flags,
            const VkCommandBufferInheritanceInfo* pInheritanceInfo);

/**@}*/

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef VULKANX_COMMAND_BUFFER_H
