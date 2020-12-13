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

// Find memory type index.
uint32_t vkxFindMemoryTypeIndex(
            VkPhysicalDevice physicalDevice,
            VkMemoryPropertyFlags memoryPropertyFlags,
                         uint32_t memoryTypeBits)
{
    // Memory properties.
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    // Memory types.
    for (uint32_t memoryTypeIndex = 0;
                  memoryTypeIndex < 
                  memoryProperties.memoryTypeCount;
                  memoryTypeIndex++) {
        if ((memoryTypeBits & (1 << memoryTypeIndex)) &&
            (memoryProperties.memoryTypes[
             memoryTypeIndex].propertyFlags & 
                    memoryPropertyFlags) ==
                    memoryPropertyFlags) {
            // Found.
            return memoryTypeIndex;
        }
    }

    // Not found.
    return UINT32_MAX;
}

// Allocate shared memory.
VkResult vkxAllocateSharedMemory(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            uint32_t memoryRequirementCount,
            const VkMemoryRequirements* pMemoryRequirements,
            const VkMemoryPropertyFlags* pMemoryPropertyFlags,
            const VkAllocationCallbacks* pAllocator,
            VkxSharedDeviceMemory* pSharedMemory)
{
    assert(pMemoryRequirements || memoryRequirementCount == 0);
    assert(pSharedMemory);
    memset(pSharedMemory, 0, sizeof(VkxSharedDeviceMemory));
    if (memoryRequirementCount == 0) {
        return VK_SUCCESS;
    }

    // Memory type count.
    uint32_t memoryTypeCount;
    VkMemoryPropertyFlags memoryTypePropertyFlags[VK_MAX_MEMORY_TYPES];
    {
        // Get memory properties.
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(
                physicalDevice, 
                &memoryProperties);

        // Get memory type count.
        memoryTypeCount = 
        memoryProperties.memoryTypeCount;

        // Get memory type property flags.
        for (uint32_t memoryTypeIndex = 0;
                      memoryTypeIndex < memoryTypeCount;
                      memoryTypeIndex++) {
            memoryTypePropertyFlags[memoryTypeIndex] = 
            memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags;
        }
    }

    // Memory type info.
    typedef struct MemoryTypeInfo_ {

        // Supported requirement count.
        uint32_t supportedRequirementCount;

        // Unique memory index in shared memory unique memories array, or
        // UINT32_MAX if unused.
        uint32_t uniqueMemoryIndex;

        // Allocate info.
        VkMemoryAllocateInfo allocateInfo;
    }
    MemoryTypeInfo;

    // Memory type infos.
    MemoryTypeInfo memoryTypeInfos[VK_MAX_MEMORY_TYPES];
    for (uint32_t memoryTypeIndex = 0;
                  memoryTypeIndex < memoryTypeCount;
                  memoryTypeIndex++) {
        // Initialize.
        MemoryTypeInfo* pMemoryTypeInfo = &memoryTypeInfos[memoryTypeIndex];
        pMemoryTypeInfo->supportedRequirementCount = 0;
        pMemoryTypeInfo->uniqueMemoryIndex = UINT32_MAX;
        pMemoryTypeInfo->allocateInfo.sType = 
            VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        pMemoryTypeInfo->allocateInfo.pNext = NULL;
        pMemoryTypeInfo->allocateInfo.allocationSize = 0;
        pMemoryTypeInfo->allocateInfo.memoryTypeIndex = memoryTypeIndex;

        // Iterate requirements.
        for (uint32_t memoryRequirementIndex = 0;
                      memoryRequirementIndex < memoryRequirementCount;
                      memoryRequirementIndex++) {
            // Memory type supports requirement?
            if ((((uint32_t)1 << memoryTypeIndex) &
                 pMemoryRequirements[
                  memoryRequirementIndex].memoryTypeBits) &&
                (memoryTypePropertyFlags[memoryTypeIndex] &
                 pMemoryPropertyFlags[memoryRequirementIndex]) ==
                 pMemoryPropertyFlags[memoryRequirementIndex]) {

                // Increment supported requirement count.
                pMemoryTypeInfo->supportedRequirementCount++;
            }
        } 
    }

    // Allocate memory requirement type indices.
    uint32_t* pMemoryRequirementTypeIndices = 
        VKX_LOCAL_MALLOC(sizeof(uint32_t) * memoryRequirementCount);

    // Iterate requirements, select memory type index for
    // each requirement which supports the most other requirements.
    for (uint32_t memoryRequirementIndex = 0;
                  memoryRequirementIndex < memoryRequirementCount;
                  memoryRequirementIndex++) {

        uint32_t maxSupportedRequirementCount = 0;
        uint32_t maxSupportedRequirementCountIndex = UINT32_MAX;

        // Iterate memory types.
        for (uint32_t memoryTypeIndex = 0;
                      memoryTypeIndex < memoryTypeCount; 
                      memoryTypeIndex++) {

            // Memory type supports requirement?
            if ((((uint32_t)1 << memoryTypeIndex) &
                 pMemoryRequirements[
                  memoryRequirementIndex].memoryTypeBits) &&
                (memoryTypePropertyFlags[memoryTypeIndex] &
                 pMemoryPropertyFlags[memoryRequirementIndex]) ==
                 pMemoryPropertyFlags[memoryRequirementIndex])  {

                // Memory type supported requirement count.
                uint32_t supportedRequirementCount =
                    memoryTypeInfos[
                    memoryTypeIndex].supportedRequirementCount;
                // Update.
                if (maxSupportedRequirementCount < supportedRequirementCount) {
                    maxSupportedRequirementCount = supportedRequirementCount;
                    maxSupportedRequirementCountIndex = memoryTypeIndex;
                }
            }
        }

        // No suitable memory type?
        if (maxSupportedRequirementCountIndex == UINT32_MAX) {
            // Free memory requirement type indices.
            VKX_LOCAL_FREE(pMemoryRequirementTypeIndices);
            return VK_ERROR_INITIALIZATION_FAILED;
        }
        pMemoryRequirementTypeIndices[memoryRequirementIndex] = 
            maxSupportedRequirementCountIndex;
    }

    // Allocate shared memory views.
    pSharedMemory->memoryViewCount = memoryRequirementCount;
    pSharedMemory->pMemoryViews = 
        malloc(sizeof(VkxDeviceMemoryView) * memoryRequirementCount);

    // Initialize unique memory count.
    pSharedMemory->uniqueMemoryCount = 0;

    // Iterate memory requirements.
    for (uint32_t memoryRequirementIndex = 0;
                  memoryRequirementIndex < memoryRequirementCount;
                  memoryRequirementIndex++) {

        // Memory type index for this requirement.
        uint32_t memoryTypeIndex = 
            pMemoryRequirementTypeIndices[memoryRequirementIndex];

        // Memory type info.
        MemoryTypeInfo* pMemoryTypeInfo = 
            &memoryTypeInfos[memoryTypeIndex];

        // Unique memory type?
        if (pMemoryTypeInfo->uniqueMemoryIndex == UINT32_MAX) {
            pMemoryTypeInfo->uniqueMemoryIndex =
                pSharedMemory->uniqueMemoryCount++;
        }

        // Round memory type allocation size up to required alignment.
        if (pMemoryTypeInfo->allocateInfo.allocationSize %
            pMemoryRequirements[memoryRequirementIndex].alignment) {
            pMemoryTypeInfo->allocateInfo.allocationSize =
                pMemoryTypeInfo->allocateInfo.allocationSize -
                pMemoryTypeInfo->allocateInfo.allocationSize %
                pMemoryRequirements[memoryRequirementIndex].alignment +
                pMemoryRequirements[memoryRequirementIndex].alignment;
        }

        // Initialize memory view offset.
        pSharedMemory->
            pMemoryViews[memoryRequirementIndex].offset = 
            pMemoryTypeInfo->allocateInfo.allocationSize;

        // Initialize memory view size.
        pSharedMemory->
            pMemoryViews[memoryRequirementIndex].size = 
            pMemoryRequirements[memoryRequirementIndex].size;

        // Increment memory type allocation size.
        pMemoryTypeInfo->allocateInfo.allocationSize +=
            pMemoryRequirements[memoryRequirementIndex].size;
    }

    // Allocate unique memories array.
    pSharedMemory->pUniqueMemories = 
        malloc(sizeof(VkDeviceMemory) * pSharedMemory->uniqueMemoryCount);
    for (uint32_t uniqueMemoryIndex = 0;
                  uniqueMemoryIndex < pSharedMemory->uniqueMemoryCount;
                  uniqueMemoryIndex++) {
        pSharedMemory->pUniqueMemories[uniqueMemoryIndex] = 
            VK_NULL_HANDLE;
    }

    // Iterate memory types.
    for (uint32_t memoryTypeIndex = 0;
                  memoryTypeIndex < memoryTypeCount;
                  memoryTypeIndex++) {
        // Memory type info.
        MemoryTypeInfo* pMemoryTypeInfo = 
                        &memoryTypeInfos[memoryTypeIndex];

        // Memory type used?
        uint32_t uniqueMemoryIndex = pMemoryTypeInfo->uniqueMemoryIndex;
        if (uniqueMemoryIndex != UINT32_MAX) {
            // Allocate memory.
            VkResult result = 
                vkAllocateMemory(
                        device,
                        &pMemoryTypeInfo->allocateInfo, pAllocator,
                        &pSharedMemory->pUniqueMemories[uniqueMemoryIndex]);
            // Error?
            if (VKX_IS_ERROR(result)) {
                // Free memory requirement type indices.
                VKX_LOCAL_FREE(pMemoryRequirementTypeIndices);
                // Free shared memory.
                pSharedMemory->pUniqueMemories[
                    uniqueMemoryIndex] = VK_NULL_HANDLE;
                vkxFreeSharedMemory(device, pSharedMemory, pAllocator);
                return result;
            }
        }
    }

    // Iterate memory requirements.
    for (uint32_t memoryRequirementIndex = 0;
                  memoryRequirementIndex < memoryRequirementCount;
                  memoryRequirementIndex++) {
 
        // Memory type index for this requirement.
        uint32_t memoryTypeIndex = 
            pMemoryRequirementTypeIndices[memoryRequirementIndex];

        // Memory type info.
        MemoryTypeInfo* pMemoryTypeInfo = 
            &memoryTypeInfos[memoryTypeIndex];

        // Initialize memory view unique memory handle.
        pSharedMemory->pMemoryViews[memoryRequirementIndex].memory =
            pSharedMemory->pUniqueMemories[pMemoryTypeInfo->uniqueMemoryIndex];
    }

    // Free memory requirement type indices.
    VKX_LOCAL_FREE(pMemoryRequirementTypeIndices);

    return VK_SUCCESS;
}

// Free shared memory.
void vkxFreeSharedMemory(
            VkDevice device,
            VkxSharedDeviceMemory* pSharedMemory,
            const VkAllocationCallbacks* pAllocator)
{
    if (pSharedMemory) {
        for (uint32_t uniqueMemoryIndex = 0;
                      uniqueMemoryIndex < pSharedMemory->uniqueMemoryCount; 
                      uniqueMemoryIndex++) {
            // Free.
            vkFreeMemory(
                    device, 
                    pSharedMemory->pUniqueMemories[uniqueMemoryIndex], 
                    pAllocator);
        }
        // Free.
        free(pSharedMemory->pUniqueMemories);
        free(pSharedMemory->pMemoryViews);

        // Nullify.
        memset(pSharedMemory, 0, sizeof(VkxSharedDeviceMemory));
    }
}
