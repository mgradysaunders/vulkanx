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
 * @brief Dynamic descriptor pool.
 */
typedef struct VkxDynamicDescriptorPool_
{
    /**
     * @brief Descriptor pool create info.
     */
    VkDescriptorPoolCreateInfo poolCreateInfo;

    /**
     * @brief Descriptor pool count.
     */
    uint32_t poolCount;

    /**
     * @brief Descriptor pool capacity.
     */
    uint32_t poolCapacity;

    /**
     * @brief Descriptor pools.
     */
    VkDescriptorPool* pPools;

    /**
     * @brief Descriptor pool full flags.
     */
    VkBool32* pFullFlags;
}
VkxDynamicDescriptorPool;

/**
 * @brief Dynamic descriptor set.
 */
typedef struct VkxDynamicDescriptorSet_
{
    /**
     * @brief Descriptor set.
     */
    VkDescriptorSet set;

    /**
     * @brief Associated descriptor pool index.
     */
    uint32_t associatedPoolIndex;
}
VkxDynamicDescriptorSet;

/**
 * @brief Create dynamic descriptor pool.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] pPoolCreateInfo
 * Descriptor pool create info.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[out] pDynamicPool
 * Dynamic descriptor pool.
 */
VkResult vkxCreateDynamicDescriptorPool(
            VkDevice device,
            const VkDescriptorPoolCreateInfo* pPoolCreateInfo,
            const VkAllocationCallbacks* pAllocator,
            VkxDynamicDescriptorPool* pDynamicPool)
                __attribute__((nonnull(1, 3)));

/**
 * @brief Allocate dynamic descriptor sets.
 *
 * @param[in] device
 * Device.
 *
 * @param[inout] pDynamicPool
 * Dynamic descriptor pool.
 *
 * @param[in] setCount
 * Descriptor set count.
 *
 * @param[in] pSetLayouts
 * Descriptor set layouts.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[out] pDynamicSets
 * Dynamic descriptor sets.
 */
VkResult vkxAllocateDynamicDescriptorSets(
            VkDevice device,
            VkxDynamicDescriptorPool* pDynamicPool,
            uint32_t setCount,
            const VkDescriptorSetLayout* pSetLayouts,
            const VkAllocationCallbacks* pAllocator,
            VkxDynamicDescriptorSet* pDynamicSets)
                __attribute__((nonnull(2)));

/**
 * @brief Free dynamic descriptor sets.
 *
 * @param[in] device
 * Device.
 *
 * @param[inout] pDynamicPool
 * Dynamic descriptor pool.
 *
 * @param[in] setCount
 * Descriptor set count.
 *
 * @param[inout] pDynamicSets
 * Dynamic descriptor sets.
 */
VkResult vkxFreeDynamicDescriptorSets(
            VkDevice device,
            VkxDynamicDescriptorPool* pDynamicPool,
            uint32_t setCount,
            VkxDynamicDescriptorSet* pDynamicSets)
                __attribute__((nonnull(2)));

/**
 * @brief Destroy dynamic descriptor pool.
 *
 * @param[in] device
 * Device.
 *
 * @param[inout] pDynamicPool
 * Dynamic descriptor pool.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 */
void vkxDestroyDynamicDescriptorPool(
            VkDevice device,
            VkxDynamicDescriptorPool* pDynamicPool,
            const VkAllocationCallbacks* pAllocator);

/**@}*/

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef VULKANX_DESCRIPTOR_SET_H
