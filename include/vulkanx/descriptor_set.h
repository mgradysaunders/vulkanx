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
#ifndef VULKANX_DESCRIPTOR_SET_H
#define VULKANX_DESCRIPTOR_SET_H

#include <vulkan/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/**
 * @defgroup descriptor_set Descriptor set
 *
 * `<vulkanx/descriptor_set.h>`
 */
/**@{*/

/**
 * @brief Descriptor set group.
 */
typedef struct VkxDescriptorSetGroup_
{
    /**
     * @brief Set layout count.
     */
    uint32_t setLayoutCount;

    /**
     * @brief Set layouts.
     */
    VkDescriptorSetLayout* pSetLayouts;

    /**
     * @brief Set counts for each set layout.
     */
    uint32_t* pSetCounts;

    /**
     * @brief Sets for each set layout.
     */
    VkDescriptorSet** ppSets;

    /**
     * @brief Pool.
     */
    VkDescriptorPool pool;
}
VkxDescriptorSetGroup;

/**
 * @brief Create descriptor set group.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] setLayoutCount
 * Set layout count.
 *
 * @param[in] pSetLayoutCreateInfos
 * Set layout create infos.
 *
 * @param[in] pSetCounts
 * Set counts for each set layout.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[out] pSetGroup
 * Set group.
 */
VkResult vkxCreateDescriptorSetGroup(
            VkDevice device,
            uint32_t setLayoutCount,
            const VkDescriptorSetLayoutCreateInfo* pSetLayoutCreateInfos,
            const uint32_t* pSetCounts,
            const VkAllocationCallbacks* pAllocator,
            VkxDescriptorSetGroup* pSetGroup) __attribute__((nonnull(6)));

/**
 * @brief Destroy descriptor set group.
 *
 * @param[in] device
 * Device.
 *
 * @param[inout] pSetGroup
 * Set group.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 */
void vkxDestroyDescriptorSetGroup(
            VkDevice device,
            VkxDescriptorSetGroup* pSetGroup,
            const VkAllocationCallbacks* pAllocator);

/**@}*/

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef VULKANX_DESCRIPTOR_SET_H
