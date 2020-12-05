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
#include <vulkanx/result.h>
#include <vulkanx/command_buffer.h>

// Create default fences.
VkResult vkxCreateDefaultFences(
                VkDevice device,
                uint32_t fenceCount,
                const VkAllocationCallbacks* pAllocator,
                VkFence* pFences)
{
    if (fenceCount == 0) {
        return VK_SUCCESS;
    }

    assert(pFences);

    for (uint32_t fenceIndex = 0;
                  fenceIndex < fenceCount;
                  fenceIndex++) {
        // Nullify.
        pFences[fenceIndex] = VK_NULL_HANDLE;
    }

    // Default fence create info.
    VkFenceCreateInfo fenceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0
    };

    VkResult result = VK_SUCCESS;
    for (uint32_t fenceIndex = 0;
                  fenceIndex < fenceCount;
                  fenceIndex++) {
        // Create fence.
        result = 
            vkCreateFence(
                    device, 
                    &fenceCreateInfo, pAllocator, 
                    &pFences[fenceIndex]);
        if (VKX_IS_ERROR(result)) {
            pFences[fenceIndex] = VK_NULL_HANDLE;
            break;
        }
    }

    // Error?
    if (VKX_IS_ERROR(result)) {
        for (uint32_t fenceIndex = 0;
                      fenceIndex < fenceCount;
                      fenceIndex++) {
            // Destroy.
            vkDestroyFence(
                    device,
                    pFences[fenceIndex],
                    pAllocator);
            // Nullify.
            pFences[fenceIndex] = VK_NULL_HANDLE;
        }
    }
    return result;
}

// Create default semaphores.
VkResult vkxCreateDefaultSemaphores(
                VkDevice device,
                uint32_t semaphoreCount,
                const VkAllocationCallbacks* pAllocator,
                VkSemaphore* pSemaphores)
{
    if (semaphoreCount == 0) {
        return VK_SUCCESS;
    }

    assert(pSemaphores);

    for (uint32_t semaphoreIndex = 0;
                  semaphoreIndex < semaphoreCount;
                  semaphoreIndex++) {
        // Nullify.
        pSemaphores[semaphoreIndex] = VK_NULL_HANDLE;
    }

    // Default semaphore create info.
    VkSemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0
    };

    VkResult result = VK_SUCCESS;
    for (uint32_t semaphoreIndex = 0;
                  semaphoreIndex < semaphoreCount;
                  semaphoreIndex++) {
        // Create fence.
        result = 
            vkCreateSemaphore(
                    device, 
                    &semaphoreCreateInfo, pAllocator, 
                    &pSemaphores[semaphoreIndex]);
        if (VKX_IS_ERROR(result)) {
            pSemaphores[semaphoreIndex] = VK_NULL_HANDLE;
            break;
        }
    }

    // Error?
    if (VKX_IS_ERROR(result)) {
        for (uint32_t semaphoreIndex = 0;
                      semaphoreIndex < semaphoreCount;
                      semaphoreIndex++) {
            // Destroy.
            vkDestroySemaphore(
                    device, 
                    pSemaphores[semaphoreIndex], 
                    pAllocator);
            // Nullify.
            pSemaphores[semaphoreIndex] = VK_NULL_HANDLE;
        }
    }
    return result;
}

// Allocate and begin command buffers.
VkResult vkxAllocateAndBeginCommandBuffers(
            VkDevice device,
            const VkCommandBufferAllocateInfo* pAllocateInfo,
            const VkCommandBufferBeginInfo* pBeginInfos,
            VkCommandBuffer* pCommandBuffers) 
{
    assert(pAllocateInfo);
    if (pAllocateInfo->commandBufferCount == 0) {
        return VK_SUCCESS;
    }

    // Sanity check.
    assert(pBeginInfos && pCommandBuffers);

    VkResult result;
    // Allocate command buffers.
    result = 
        vkAllocateCommandBuffers(
                device,
                pAllocateInfo,
                pCommandBuffers);
    // Allocate command buffers error?
    if (VKX_IS_ERROR(result)) {
        // Failure.
        goto FAILURE;
    }

    for (uint32_t commandBufferIndex = 0;
                  commandBufferIndex < pAllocateInfo->commandBufferCount;
                  commandBufferIndex++) {
        // Begin command buffer.
        result = 
            vkBeginCommandBuffer(
                    pCommandBuffers[commandBufferIndex],
                    &pBeginInfos[commandBufferIndex]);
        // Begin command buffer error?
        if (VKX_IS_ERROR(result)) {
            // Free command buffers.
            vkFreeCommandBuffers(
                    device,
                    pAllocateInfo->commandPool,
                    pAllocateInfo->commandBufferCount,
                    pCommandBuffers);
            // Failure.
            goto FAILURE;
        }
    }

    // Success.
    return VK_SUCCESS;

FAILURE:

    // Nullify.
    for (uint32_t commandBufferIndex = 0;
                  commandBufferIndex < pAllocateInfo->commandBufferCount;
                  commandBufferIndex++) {
        pCommandBuffers[commandBufferIndex] = VK_NULL_HANDLE;
    }
    return result;
}

// Flush command buffers.
VkResult vkxFlushCommandBuffers(
            VkDevice device,
            VkQueue queue,
            uint32_t commandBufferCount,
            const VkCommandBuffer* pCommandBuffers,
            const VkAllocationCallbacks* pAllocator)
{
    VkFence fence = VK_NULL_HANDLE;

    {
        // Create fence.
        VkFenceCreateInfo fenceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0
        };
        VkResult result = 
            vkCreateFence(
                    device,
                    &fenceCreateInfo, pAllocator,
                    &fence);
        if (VKX_IS_ERROR(result)) {
            return result;
        }
    }

    {
        // Submit.
        VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = NULL,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = NULL,
            .pWaitDstStageMask = NULL,
            .commandBufferCount = commandBufferCount,
            .pCommandBuffers = pCommandBuffers,
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = NULL
        };
        VkResult result = vkQueueSubmit(queue, 1, &submitInfo, fence);
        if (VKX_IS_ERROR(result)) {
            // Destroy fence.
            vkDestroyFence(device, fence, pAllocator);
            return result;
        }
    }

    // Wait.
    VkResult result = 
        vkWaitForFences(
                device,
                1, 
                &fence,
                VK_TRUE,
                UINT64_MAX);

    // Destroy fence.
    vkDestroyFence(device, fence, pAllocator);

    return result;
}

// End, flush, and free command buffers.
VkResult vkxEndFlushAndFreeCommandBuffers(
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            uint32_t commandBufferCount,
            const VkCommandBuffer* pCommandBuffers,
            const VkAllocationCallbacks* pAllocator)
{
    if (commandBufferCount == 0) {
        return VK_SUCCESS;
    }

    assert(pCommandBuffers);

    for (uint32_t commandBufferIndex = 0;
                  commandBufferIndex < commandBufferCount;
                  commandBufferIndex++) {
        // End.
        vkEndCommandBuffer(pCommandBuffers[commandBufferIndex]);
    }

    // Flush.
    VkResult result =
        vkxFlushCommandBuffers(
                device,
                queue,
                commandBufferCount,
                pCommandBuffers,
                pAllocator);

    // Free.
    vkFreeCommandBuffers(
            device,
            commandPool,
            commandBufferCount,
            pCommandBuffers);

    return result;
}


