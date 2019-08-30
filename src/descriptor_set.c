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
#include <string.h>
#include <stdlib.h>
#include <vulkanx/memory.h>
#include <vulkanx/result.h>
#include <vulkanx/descriptor_set.h>

// Create dynamic descriptor pool.
VkResult vkxCreateDynamicDescriptorPool(
            VkDevice device,
            const VkDescriptorPoolCreateInfo* pPoolCreateInfo,
            const VkAllocationCallbacks* pAllocator,
            VkxDynamicDescriptorPool* pDynamicPool)
{
    assert(pPoolCreateInfo);
    assert(pDynamicPool);
    memset(pDynamicPool, 0, sizeof(VkxDynamicDescriptorPool));

    VkDescriptorPool initialPool;
    VkResult result = 
        vkCreateDescriptorPool(
                device,
                pPoolCreateInfo,
                pAllocator,
                &initialPool);
    if (VKX_IS_ERROR(result)) {
        return result;
    }

    // Deep-copy pool sizes.
    VkDescriptorPoolSize* pPoolSizes = 
        (VkDescriptorPoolSize*)malloc(
                sizeof(VkDescriptorPoolSize) * pPoolCreateInfo->poolSizeCount);
    for (uint32_t poolSizeIndex = 0;
                  poolSizeIndex < pPoolCreateInfo->poolSizeCount;
                  poolSizeIndex++) {
        pPoolSizes[poolSizeIndex] = 
            pPoolCreateInfo->pPoolSizes[poolSizeIndex];
    }

    // Initialize.
    pDynamicPool->poolCreateInfo = *pPoolCreateInfo;
    pDynamicPool->poolCreateInfo.pPoolSizes = pPoolSizes;
    pDynamicPool->poolCount = 1;
    pDynamicPool->poolCapacity = 4;
    pDynamicPool->pPools = 
        (VkDescriptorPool*)malloc(sizeof(VkDescriptorPool) * 4);
    pDynamicPool->pPools[0] = initialPool;
    pDynamicPool->pFullFlags = 
        (VkBool32*)malloc(sizeof(VkBool32) * 4);
    pDynamicPool->pFullFlags[0] = VK_FALSE;
    return result;
}

// Allocate dynamic descriptor sets.
VkResult vkxAllocateDynamicDescriptorSets(
            VkDevice device,
            VkxDynamicDescriptorPool* pDynamicPool,
            uint32_t setCount,
            const VkDescriptorSetLayout* pSetLayouts,
            const VkAllocationCallbacks* pAllocator,
            VkxDynamicDescriptorSet* pDynamicSets)
{
    assert(pDynamicPool);
    if (setCount == 0) {
        return VK_SUCCESS;
    }

    assert(pSetLayouts && pDynamicSets);

    for (uint32_t setIndex = 0;
                  setIndex < setCount; setIndex++) {
        // Nullify.
        pDynamicSets[setIndex].set = VK_NULL_HANDLE;
        pDynamicSets[setIndex].associatedPoolIndex = 0;
    }

    if (setCount > pDynamicPool->poolCreateInfo.maxSets) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // Descriptor set allocate info.
    VkDescriptorSetAllocateInfo setAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = VK_NULL_HANDLE, // Uninitialized.
        .descriptorSetCount = setCount,
        .pSetLayouts = pSetLayouts
    };

    // Allocate descriptor set array.
    VkDescriptorSet* pSets =
        (VkDescriptorSet*)VKX_LOCAL_MALLOC(
                sizeof(VkDescriptorSet) * setCount);

    uint32_t associatedPoolIndex = UINT32_MAX;
    for (uint32_t poolIndex = 0;
                  poolIndex < pDynamicPool->poolCount; poolIndex++) {
        // Pool not full?
        if (pDynamicPool->pFullFlags[poolIndex] == VK_FALSE) {

            // Allocate descriptor sets.
            setAllocateInfo.descriptorPool = pDynamicPool->pPools[poolIndex];
            VkResult result = 
                vkAllocateDescriptorSets(
                        device,
                        &setAllocateInfo,
                        pSets);

            // Allocate descriptor sets error?
            if (VKX_IS_ERROR(result)) {
                // Pool fragmented or out of memory?
                if (result == VK_ERROR_FRAGMENTED_POOL ||
                    result == VK_ERROR_OUT_OF_POOL_MEMORY) {
                    // Flag as full.
                    pDynamicPool->pFullFlags[poolIndex] = VK_TRUE;
                }
                else {
                    // Free descriptor set array.
                    VKX_LOCAL_FREE(pSets);
                    // Return.
                    return result;
                }
            }
            else {
                associatedPoolIndex = poolIndex;
                break;
            }
        }
    }

    if (associatedPoolIndex == UINT32_MAX) {
        associatedPoolIndex = pDynamicPool->poolCount;
        VkDescriptorPool pool = VK_NULL_HANDLE;
        {
            // Create descriptor pool.
            VkResult result = 
                vkCreateDescriptorPool(
                        device,
                        &pDynamicPool->poolCreateInfo,
                        pAllocator,
                        &pool);
            // Create descriptor pool error?
            if (VKX_IS_ERROR(result)) {
                // Free descriptor set array.
                VKX_LOCAL_FREE(pSets);
                // Return.
                return result;
            }
        }

        {
            // Allocate descriptor sets.
            setAllocateInfo.descriptorPool = pool;
            VkResult result = 
                vkAllocateDescriptorSets(
                        device,
                        &setAllocateInfo,
                        pSets);
            // Allocate descriptor set error?
            if (VKX_IS_ERROR(result)) {
                // Free descriptor set array.
                VKX_LOCAL_FREE(pSets);
                // Destroy descriptor pool.
                vkDestroyDescriptorPool(device, pool, pAllocator);
                // Return.
                return result;
            }
        }

        // Pool capacity equal to pool count?
        if (pDynamicPool->poolCapacity == pDynamicPool->poolCount) {
            pDynamicPool->poolCapacity *= 2;
            pDynamicPool->pPools = 
                (VkDescriptorPool*)realloc(
                        pDynamicPool->pPools, 
                        sizeof(VkDescriptorPool) * pDynamicPool->poolCapacity);
            pDynamicPool->pFullFlags = 
                (VkBool32*)realloc(
                        pDynamicPool->pFullFlags,
                        sizeof(VkBool32) * pDynamicPool->poolCapacity);
        }

        // Push descriptor pool.
        pDynamicPool->pPools[pDynamicPool->poolCount] = pool;
        pDynamicPool->pFullFlags[pDynamicPool->poolCount] = VK_FALSE;
        pDynamicPool->poolCount++;
    }

    // Initialize dynamic descriptor sets.
    for (uint32_t setIndex = 0;
                  setIndex < setCount; setIndex++) {
        pDynamicSets[setIndex].set = pSets[setIndex];
        pDynamicSets[setIndex].associatedPoolIndex = associatedPoolIndex;
    }

    // Free descriptor set array.
    VKX_LOCAL_FREE(pSets);

    return VK_SUCCESS;
}

// Free dynamic descriptor sets.
VkResult vkxFreeDynamicDescriptorSets(
            VkDevice device,
            VkxDynamicDescriptorPool* pDynamicPool,
            uint32_t setCount,
            VkxDynamicDescriptorSet* pDynamicSets)
{
    assert(pDynamicPool);
    if (setCount == 0) {
        return VK_SUCCESS;
    }

    assert(pDynamicSets);

    // Allocate descriptor set array.
    VkDescriptorSet* pSets = 
        (VkDescriptorSet*)VKX_LOCAL_MALLOC(
                sizeof(VkDescriptorSet) * setCount);
    // Initialize descriptor set array.
    for (uint32_t setIndex = 0;
                  setIndex < setCount; setIndex++) {
        pSets[setIndex] = pDynamicSets[setIndex].set;
    }

    for (uint32_t setIndexBegin = 0;
                  setIndexBegin < setCount;) {
        // Find set index range with same associated pool index.
        uint32_t setIndexEnd = setIndexBegin + 1;
        for (; setIndexEnd < setCount &&
                pDynamicSets[setIndexBegin].associatedPoolIndex ==
                pDynamicSets[setIndexEnd].associatedPoolIndex; setIndexEnd++) {
        }

        // Free descriptor sets.
        VkResult result = 
            vkFreeDescriptorSets(
                    device,
                    pDynamicPool->pPools[
                    pDynamicSets[setIndexBegin].associatedPoolIndex],
                    setIndexEnd -
                    setIndexBegin,
                    &pSets[setIndexBegin]);

        // Free descriptor sets error?
        if (VKX_IS_ERROR(result)) {
            // Free descriptor set array.
            VKX_LOCAL_FREE(pSets);
            // Return.
            return result;
        }

        // Flag as not full.
        pDynamicPool->pFullFlags[
        pDynamicSets[setIndexBegin].associatedPoolIndex] = VK_FALSE;
        for (; setIndexBegin < setIndexEnd; setIndexBegin++) {
            // Nullify.
            pDynamicSets[setIndexBegin].set = VK_NULL_HANDLE;
            pDynamicSets[setIndexBegin].associatedPoolIndex = 0;
        }
    }

    // Free descriptor set array.
    VKX_LOCAL_FREE(pSets);
    return VK_SUCCESS;
}

// Destroy dynamic descriptor pool.
void vkxDestroyDynamicDescriptorPool(
            VkDevice device,
            VkxDynamicDescriptorPool* pDynamicPool,
            const VkAllocationCallbacks* pAllocator)
{
    if (pDynamicPool) {
        // Free.
        free((void*)pDynamicPool->poolCreateInfo.pPoolSizes);

        for (uint32_t poolIndex = 0;
                      poolIndex < pDynamicPool->poolCount;
                      poolIndex++) {
            // Destroy.
            vkDestroyDescriptorPool(
                    device,
                    pDynamicPool->pPools[poolIndex],
                    pAllocator);
        }

        // Free.
        free(pDynamicPool->pPools);

        // Free.
        free(pDynamicPool->pFullFlags);

        // Nullify.
        memset(pDynamicPool, 0, sizeof(VkxDynamicDescriptorPool));
    }
}
