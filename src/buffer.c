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

// Create buffer.
VkResult vkxCreateBuffer(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            const VkBufferCreateInfo* pBufferCreateInfo,
            const VkMemoryPropertyFlags memoryPropertyFlags,
            const VkAllocationCallbacks* pAllocator,
            VkxBuffer* pBuffer)
{
    assert(pBufferCreateInfo);
    assert(pBuffer);

    // Nullify.
    pBuffer->buffer = VK_NULL_HANDLE;
    pBuffer->memory = VK_NULL_HANDLE;
    
    {
        // Create buffer.
        VkResult result = 
            vkCreateBuffer(
                    device,
                    pBufferCreateInfo, pAllocator,
                    &pBuffer->buffer);
        if (VKX_IS_ERROR(result)) {
            pBuffer->buffer = VK_NULL_HANDLE;
            return result;
        }
    }

    {
        // Get memory requirements.
        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(
                device,
                pBuffer->buffer,
                &memoryRequirements);

        // Find memory type index.
        uint32_t memoryTypeIndex = 
            vkxFindMemoryTypeIndex(
                    physicalDevice,
                    memoryPropertyFlags,
                    memoryRequirements.memoryTypeBits);
        if (memoryTypeIndex == UINT32_MAX) {
            // Destroy buffer.
            vkxDestroyBuffer(device, pBuffer, pAllocator);
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
                    &pBuffer->memory);
        if (VKX_IS_ERROR(result)) {
            // Destroy buffer.
            pBuffer->memory = VK_NULL_HANDLE;
            vkxDestroyBuffer(device, pBuffer, pAllocator);
            return result;
        }
    }

    // Bind memory.
    VkResult result = 
        vkBindBufferMemory(
                device,
                pBuffer->buffer,
                pBuffer->memory,
                0);
    if (VKX_IS_ERROR(result)) {
        // Destroy buffer.
        vkxDestroyBuffer(device, pBuffer, pAllocator);
        // Fall through.
    }
    return result;
}

// Destroy buffer.
void vkxDestroyBuffer(
            VkDevice device,
            VkxBuffer* pBuffer,
            const VkAllocationCallbacks* pAllocator)
{
    if (pBuffer) {
        // Destroy buffer.
        vkDestroyBuffer(
                device, 
                pBuffer->buffer, 
                pAllocator);

        // Free memory.
        vkFreeMemory(
                device, 
                pBuffer->memory, 
                pAllocator);

        // Nullify.
        pBuffer->buffer = VK_NULL_HANDLE;
        pBuffer->memory = VK_NULL_HANDLE;
    }
}

// Create buffer group.
VkResult vkxCreateBufferGroup(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            uint32_t bufferCount,
            const VkBufferCreateInfo* pBufferCreateInfos,
            const VkMemoryPropertyFlags* pMemoryPropertyFlags,
            const VkAllocationCallbacks* pAllocator,
            VkxBufferGroup* pBufferGroup)
{
    assert(pBufferGroup);
    memset(pBufferGroup, 0, sizeof(VkxBufferGroup));
    if (bufferCount == 0) {
        return VK_SUCCESS;
    }

    // Sanity check.
    assert(pBufferCreateInfos);
    assert(pMemoryPropertyFlags);

    // Allocate buffers.
    pBufferGroup->bufferCount = bufferCount;
    pBufferGroup->pBuffers = 
        (VkBuffer*)malloc(sizeof(VkBuffer) * bufferCount);
    VkBuffer* pBuffers = pBufferGroup->pBuffers;
    for (uint32_t bufferIndex = 0;
                  bufferIndex < bufferCount; bufferIndex++) {
        pBuffers[bufferIndex] = VK_NULL_HANDLE;
    }

    // Allocate memory requirements.
    VkMemoryRequirements* pMemoryRequirements = 
        (VkMemoryRequirements*)VKX_LOCAL_MALLOC(
                sizeof(VkMemoryRequirements) * bufferCount);

    // Iterate buffers.
    for (uint32_t bufferIndex = 0; 
                  bufferIndex < bufferCount;
                  bufferIndex++) {
        // Create buffer.
        VkResult result = 
            vkCreateBuffer(
                    device,
                    &pBufferCreateInfos[bufferIndex], pAllocator,
                    &pBuffers[bufferIndex]);
        if (VKX_IS_ERROR(result)) {
            // Free memory requirements.
            VKX_LOCAL_FREE(pMemoryRequirements);
            // Destroy buffer group.
            pBuffers[bufferIndex] = VK_NULL_HANDLE;
            vkxDestroyBufferGroup(device, pBufferGroup, pAllocator);
            return result;
        }

        // Get buffer memory requirements.
        vkGetBufferMemoryRequirements(
                device, 
                pBuffers[bufferIndex],
                &pMemoryRequirements[bufferIndex]);
    }

    {
        // Allocate shared memory.
        VkResult result =
            vkxAllocateSharedMemory(
                    physicalDevice,
                    device,
                    bufferCount,
                    pMemoryRequirements,
                    pMemoryPropertyFlags,
                    pAllocator,
                    &pBufferGroup->sharedMemory);
        if (VKX_IS_ERROR(result)) {
            // Free memory requirements.
            VKX_LOCAL_FREE(pMemoryRequirements);
            // Destroy buffer group.
            vkxDestroyBufferGroup(device, pBufferGroup, pAllocator);
            return result;
        }
    }

    // Bind memory.
    VkxDeviceMemoryView* pMemoryViews = 
        pBufferGroup->sharedMemory.pMemoryViews;
    for (uint32_t bufferIndex = 0;
                  bufferIndex < bufferCount;
                  bufferIndex++) {
        VkResult result = 
            vkBindBufferMemory(
                    device,
                    pBuffers[bufferIndex],
                    pMemoryViews[bufferIndex].memory,
                    pMemoryViews[bufferIndex].offset);
        if (VKX_IS_ERROR(result)) {
            // Free memory requirements.
            VKX_LOCAL_FREE(pMemoryRequirements);
            // Destroy buffer group.
            vkxDestroyBufferGroup(device, pBufferGroup, pAllocator);
            return result;
        }
    }

    // Free memory requirements.
    VKX_LOCAL_FREE(pMemoryRequirements);

    return VK_SUCCESS;
}

// Destroy buffer group.
void vkxDestroyBufferGroup(
            VkDevice device,
            VkxBufferGroup* pBufferGroup,
            const VkAllocationCallbacks* pAllocator)
{
    if (pBufferGroup) {
        for (uint32_t bufferIndex = 0;
                      bufferIndex < pBufferGroup->bufferCount;
                      bufferIndex++) {
            // Destroy buffer.
            vkDestroyBuffer(
                    device,
                    pBufferGroup->pBuffers[bufferIndex], 
                    pAllocator);
        }
        // Free buffers.
        free(pBufferGroup->pBuffers);

        // Nullify.
        pBufferGroup->pBuffers = NULL;
        pBufferGroup->bufferCount = 0;

        // Delegate.
        vkxFreeSharedMemory(device, &pBufferGroup->sharedMemory, pAllocator);
    }
}

#if 0
// Get buffer data.
VkResult vkxGetBufferData(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            const VkAllocationCallbacks* pAllocator,
            const VkxBufferDataAccessInfo* pAccessInfo,
            VkDeviceSize dataSize,
            void* pData)
{
    assert(pAccessInfo);
    assert(pData || dataSize == 0);
    if (dataSize == 0) {
        return VK_SUCCESS;
    }

    VkxBuffer dstBuffer;
    {
        // Create staging buffer.
        VkBufferCreateInfo bufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .size = dataSize,
            .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = NULL
        };
        VkResult result = 
            vkxCreateBuffer(
                    physicalDevice,
                    device,
                    &bufferCreateInfo,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    pAllocator,
                    &dstBuffer);
        if (VKX_IS_ERROR(result)) {
            return result;
        }
    }

    VkCommandBuffer commandBuffer;
    {
        // Allocate command buffer.
        VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = NULL,
            .commandPool = commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };
        VkResult result = 
            vkAllocateCommandBuffers(
                    device,
                    &commandBufferAllocateInfo,
                    &commandBuffer);
        if (VKX_IS_ERROR(result)) {
            // Destroy staging buffer.
            vkxDestroyBuffer(device, &dstBuffer, pAllocator);
            return result;
        }
    }

    {
        // Begin command buffer.
        VkCommandBufferBeginInfo commandBufferBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = NULL,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = NULL
        };
        VkResult result = 
            vkBeginCommandBuffer(
                    commandBuffer,
                    &commandBufferBeginInfo);
        if (VKX_IS_ERROR(result)) {
            // Free command buffer.
            vkFreeCommandBuffers(
                    device, 
                    commandPool, 
                    1, &commandBuffer);
            // Destroy staging buffer.
            vkxDestroyBuffer(device, &dstBuffer, pAllocator);
            return result;
        }
    }

    {
        // Buffer copy.
        VkBufferCopy bufferCopy = {
            .srcOffset = pAccessInfo->offset,
            .dstOffset = 0,
            .size = dataSize
        };
        vkCmdCopyBuffer(
                commandBuffer,
                pAccessInfo->buffer,
                dstBuffer.buffer,
                1, &bufferCopy);
    }

    // End command buffer.
    vkEndCommandBuffer(commandBuffer);

    {
        // Flush command buffer.
        VkResult result = 
            vkxFlushCommandBuffers(
                    device, 
                    queue, 
                    1, &commandBuffer, 
                    pAllocator);
        if (VKX_IS_ERROR(result)) {
            // Free command buffer.
            vkFreeCommandBuffers(
                    device, 
                    commandPool,
                    1, &commandBuffer);
            // Destroy staging buffer.
            vkxDestroyBuffer(device, &dstBuffer, pAllocator);
            return result;
        }
    }

    // Read from staging buffer.
    void* pMappedData = NULL;
    VkResult result = 
        vkMapMemory(
                device,
                dstBuffer.memory,
                0, dataSize,
                0, &pMappedData);
    if (VKX_IS_OK(result)) {
        memcpy(pData, pMappedData, dataSize);
        vkUnmapMemory(device, dstBuffer.memory);
        // Fall through.
    }

    // Free command buffer.
    vkFreeCommandBuffers(
            device, 
            commandPool, 
            1, &commandBuffer);

    // Destroy staging buffer.
    vkxDestroyBuffer(device, &dstBuffer, pAllocator);

    return result;
}

// Set buffer data.
VkResult vkxSetBufferData(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VkQueue queue,
            VkCommandPool commandPool,
            const VkAllocationCallbacks* pAllocator,
            const VkxBufferDataAccessInfo* pAccessInfo,
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
        .flags = 0,
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
    void* pMappedData = NULL;
    (void)
    VKX_VERIFIED_CALL(
        vkMapMemory,
                device,
                srcBuffer.memory,
                (VkDeviceSize)0, dataSize,
                (VkMemoryMapFlags)0, 
                &pMappedData);
    memcpy(pMappedData, pData, dataSize);
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

    // Buffer copy.
    VkBufferCopy bufferCopy = {
        .srcOffset = 0,
        .dstOffset = pAccessInfo->offset,
        .size = dataSize
    };
    vkCmdCopyBuffer(
            commandBuffer,
            srcBuffer.buffer,
            pAccessInfo->buffer,
            1, &bufferCopy);

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
    vkxDestroyBuffer(device, &srcBuffer, pAllocator);
}
#endif
