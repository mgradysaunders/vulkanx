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
#include <vulkanx/memory.h>
#include <vulkanx/result.h>
#include <vulkanx/descriptor_set.h>

// Create descriptor set group.
VkResult vkxCreateDescriptorSetGroup(
            VkDevice device,
            uint32_t setLayoutCount,
            const VkDescriptorSetLayoutCreateInfo* pSetLayoutCreateInfos,
            const uint32_t* pSetCounts,
            const VkAllocationCallbacks* pAllocator,
            VkxDescriptorSetGroup* pSetGroup)
{
    assert(pSetGroup);
    memset(pSetGroup, 0, sizeof(VkxDescriptorSetGroup));
    if (setLayoutCount == 0) {
        return VK_SUCCESS;
    }

    // Sanity check.
    assert(pSetLayoutCreateInfos);
    assert(pSetCounts);

    uint32_t totalSetCount = 0;
    for (uint32_t setLayoutIndex = 0;
                  setLayoutIndex < setLayoutCount;
                  setLayoutIndex++) {
        totalSetCount += pSetCounts[setLayoutIndex];
        if (pSetCounts[setLayoutIndex] == 0) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }
    }

    {
        // Count descriptors.
        uint32_t descriptorCounts[16];
        for (uint32_t typeIndex = 0; 
                      typeIndex < 16; typeIndex++) {
            descriptorCounts[typeIndex] = 0;
        }
        for (uint32_t setLayoutIndex = 0;
                      setLayoutIndex < setLayoutCount;
                      setLayoutIndex++) {
            uint32_t setCount = pSetCounts[setLayoutIndex];
            const VkDescriptorSetLayoutCreateInfo* 
                pSetLayoutCreateInfo = 
                &pSetLayoutCreateInfos[setLayoutIndex];
            for (uint32_t bindingIndex = 0;
                          bindingIndex < pSetLayoutCreateInfo->bindingCount;
                          bindingIndex++) {
                const VkDescriptorSetLayoutBinding* 
                    pBinding = 
                    &pSetLayoutCreateInfo->pBindings[bindingIndex];
                descriptorCounts[pBinding->descriptorType] += 
                                 pBinding->descriptorCount * setCount;
            }
        }

        // Initialize descriptor pool sizes.
        uint32_t poolSizeCount = 0;
        VkDescriptorPoolSize poolSizes[16];
        for (uint32_t typeIndex = 0; 
                      typeIndex < 16; typeIndex++) {
            if (descriptorCounts[typeIndex] > 0) {
                poolSizes[poolSizeCount].type = typeIndex;
                poolSizes[poolSizeCount].descriptorCount = 
                    descriptorCounts[typeIndex];
                poolSizeCount++;
            }
        }

        // Descriptor pool create info.
        VkDescriptorPoolCreateInfo poolCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .maxSets = totalSetCount,
            .poolSizeCount = poolSizeCount,
            .pPoolSizes = &poolSizes[0]
        };

        // Create descriptor pool.
        VkResult result = 
            vkCreateDescriptorPool(
                    device,
                    &poolCreateInfo, pAllocator,
                    &pSetGroup->pool);
        // Create descriptor pool error?
        if (VKX_IS_ERROR(result)) {
            pSetGroup->pool = VK_NULL_HANDLE;
            return result;
        }
    }

    {
        // Memory size.
        size_t memorySize = 0;

        // Memory region sizes.
        size_t regionSizes[4] = {
            setLayoutCount * sizeof(VkDescriptorSetLayout),
            setLayoutCount * sizeof(uint32_t),
            setLayoutCount * sizeof(VkDescriptorSet*),
            totalSetCount * sizeof(VkDescriptorSet)
        };

        // Memory region alignments.
        size_t regionAlignments[4] = {
            _Alignof(VkDescriptorSetLayout),
            _Alignof(uint32_t),
            _Alignof(VkDescriptorSet*),
            _Alignof(VkDescriptorSet)
        };

        // Memory region offsets.
        size_t regionOffsets[4] = {0, 0, 0, 0};

        for (size_t regionIndex = 0; 
                    regionIndex < 4; regionIndex++) {
            // Round memory size up to required alignment.
            if (memorySize % regionAlignments[regionIndex]) {
                memorySize = 
                    memorySize - 
                    memorySize % regionAlignments[regionIndex] +
                    regionAlignments[regionIndex];
            }
            // Store offset.
            regionOffsets[regionIndex] = memorySize;

            // Increment memory size.
            memorySize += regionSizes[regionIndex];
        }

        // Allocate memory.
        char* pMemory = (char*)malloc(memorySize);

        pSetGroup->setLayoutCount = setLayoutCount;

        // Link pSetLayouts.
        pSetGroup->pSetLayouts = 
            (VkDescriptorSetLayout*)pMemory;
        for (uint32_t setLayoutIndex = 0;
                      setLayoutIndex < setLayoutCount; setLayoutIndex++) {
            // Nullify.
            pSetGroup->pSetLayouts[setLayoutIndex] = VK_NULL_HANDLE;
        }

        // Link pSetCounts.
        pSetGroup->pSetCounts = 
            (uint32_t*)&pMemory[regionOffsets[1]];
        for (uint32_t setLayoutIndex = 0;
                      setLayoutIndex < setLayoutCount; setLayoutIndex++) {
            // Copy.
            pSetGroup->pSetCounts[setLayoutIndex] = pSetCounts[setLayoutIndex];
        }

        // Link ppSets.
        pSetGroup->ppSets = 
            (VkDescriptorSet**)&pMemory[regionOffsets[2]];

        // Link ppSets entries.
        VkDescriptorSet* pSets = 
            (VkDescriptorSet*)&pMemory[regionOffsets[3]];
        uint32_t setCount = 0;
        for (uint32_t setLayoutIndex = 0;
                      setLayoutIndex < setLayoutCount; setLayoutIndex++) {
            // Link.
            pSetGroup->ppSets[setLayoutIndex] = &pSets[setCount];
            setCount += pSetCounts[setLayoutIndex];
        }
    }

    for (uint32_t setLayoutIndex = 0;
                  setLayoutIndex < setLayoutCount;
                  setLayoutIndex++) {
        // Create descriptor set layout.
        VkResult result = 
            vkCreateDescriptorSetLayout(
                    device,
                    &pSetLayoutCreateInfos[setLayoutIndex], pAllocator,
                    &pSetGroup->pSetLayouts[setLayoutIndex]);
        // Create descriptor set layout error?
        if (VKX_IS_ERROR(result)) {
            // Destroy descriptor set group.
            pSetGroup->pSetLayouts[setLayoutIndex] = VK_NULL_HANDLE;
            vkxDestroyDescriptorSetGroup(device, pSetGroup, pAllocator);
            return result;
        }
    }

    // Allocate total set layouts.
    VkDescriptorSetLayout* pTotalSetLayouts =
        (VkDescriptorSetLayout*)VKX_LOCAL_MALLOC(
                sizeof(VkDescriptorSetLayout) * totalSetCount);
    {
        // Initialize total set layouts.
        VkDescriptorSetLayout* pTotalSetLayoutsItr = pTotalSetLayouts;
        for (uint32_t setLayoutIndex = 0;
                      setLayoutIndex < setLayoutCount;
                      setLayoutIndex++) {
            for (uint32_t setIndex = 0; 
                          setIndex < pSetCounts[setLayoutIndex];
                          setIndex++) {
                *pTotalSetLayoutsItr++ =
                    pSetGroup->pSetLayouts[setLayoutIndex];
            }
        }
    }

    // Descriptor set allocate info.
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = pSetGroup->pool,
        .descriptorSetCount = totalSetCount,
        .pSetLayouts = pTotalSetLayouts
    };

    // Allocate descriptor sets.
    VkResult result = 
        vkAllocateDescriptorSets(
                device,
                &descriptorSetAllocateInfo,
                &pSetGroup->ppSets[0][0]);

    // Free total set layouts.
    VKX_LOCAL_FREE(pTotalSetLayouts);

    // Allocate descriptor sets error?
    if (VKX_IS_ERROR(result)) {
        // Destroy descriptor set group.
        vkxDestroyDescriptorSetGroup(device, pSetGroup, pAllocator);
    }

    return result;
}

// Destroy descriptor set group.
void vkxDestroyDescriptorSetGroup(
            VkDevice device,
            VkxDescriptorSetGroup* pSetGroup,
            const VkAllocationCallbacks* pAllocator)
{
    if (pSetGroup) {

        for (uint32_t setLayoutIndex = 0;
                      setLayoutIndex < pSetGroup->setLayoutCount;
                      setLayoutIndex++) {

            // Destroy descriptor set layout.
            vkDestroyDescriptorSetLayout(
                    device,
                    pSetGroup->pSetLayouts[setLayoutIndex], 
                    pAllocator);
        }

        // Destroy descriptor pool.
        vkDestroyDescriptorPool(
                device,
                pSetGroup->pool,
                pAllocator);

        // Free memory.
        void* pMem = pSetGroup->pSetLayouts;
        free(pMem);

        // Nullify.
        memset(pSetGroup, 0, sizeof(VkxDescriptorSetGroup));
    }
}
