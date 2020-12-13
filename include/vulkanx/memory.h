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
#ifndef VULKANX_MEMORY_H
#define VULKANX_MEMORY_H

#include <stdlib.h>
#include <vulkan/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/**
 * @defgroup memory Memory
 *
 * `<vulkanx/memory.h>`
 */
/**@{*/

#if !DOXYGEN
#if defined(__GNUC__) && !VKX_NO_ALLOCA
#define VKX_LOCAL_MALLOC_ __builtin_alloca
#else
#define VKX_LOCAL_MALLOC_ malloc
#endif // #if defined(__GNUC__)
#endif // #if !DOXYGEN

/**
 * @brief Local malloc.
 *
 * For local allocations to use with `vkEnumerate*` functions.
 * By default, evaluates to `malloc`. If compiling with GCC, evaluates 
 * to `__builtin_alloca` to allocate on the stack. 
 */
#define VKX_LOCAL_MALLOC VKX_LOCAL_MALLOC_

#define VKX_LOCAL_MALLOC_TYPE(T, n) VKX_LOCAL_MALLOC(sizeof(T) * (n))

#if !DOXYGEN
#if defined(__GNUC__) && !VKX_NO_ALLOCA
#define VKX_LOCAL_FREE_ (void)
#else
#define VKX_LOCAL_FREE_ free
#endif // #if defined(__GNUC__)
#endif // #if !DOXYGEN

/**
 * @brief Local free.
 *
 * For local allocations to use with `vkEnumerate*` functions. 
 * By default, evaluates to `free`. If compiling with GCC, evaluates 
 * to no-op `(void)` to ignore stack-allocated pointers. 
 */
#define VKX_LOCAL_FREE VKX_LOCAL_FREE_

/**
 * @brief Find memory type index.
 *
 * @param[in] physicalDevice
 * Physical device.
 *
 * @param[in] memoryPropertyFlags
 * Memory property flags.
 *
 * @param[in] memoryTypeBits
 * Memory type bits.
 *
 * @note
 * If no suitable memory type index, returns `UINT32_MAX`.
 */
uint32_t vkxFindMemoryTypeIndex(
            VkPhysicalDevice physicalDevice,
            VkMemoryPropertyFlags memoryPropertyFlags,
                         uint32_t memoryTypeBits);

/**
 * @brief Device memory view.
 *
 * This structure represents a view of a region of `VkDeviceMemory`. This
 * structure thus contains a memory handle, the offset of the memory region 
 * in bytes, and the size of the memory region in bytes.
 */
typedef struct VkxDeviceMemoryView_
{
    /**
     * @brief Memory.
     */
    VkDeviceMemory memory;

    /**
     * @brief Region offset.
     */
    VkDeviceSize offset;

    /**
     * @brief Region size.
     */
    VkDeviceSize size;
}
VkxDeviceMemoryView;

/**
 * @brief Shared device memory.
 *
 * This structure represents a collection of `VkDeviceMemory`
 * allocations shared by a set of Vulkan objects. That is, this 
 * structure contains an array of unique `VkDeviceMemory` allocations 
 * (which it manages), and an array of `VkxDeviceMemoryView`s of its
 * unique allocations (which it also manages) to be bound to Vulkan 
 * objects.
 */
typedef struct VkxSharedDeviceMemory_
{
    /**
     * @brief Unique memory count.
     */
    uint32_t uniqueMemoryCount;

    /**
     * @brief Unique memories.
     */
    VkDeviceMemory* pUniqueMemories;

    /**
     * @brief Memory view count.
     */
    uint32_t memoryViewCount;

    /**
     * @brief Memory views.
     */
    VkxDeviceMemoryView* pMemoryViews;
}
VkxSharedDeviceMemory;

/**
 * @brief Allocate shared memory.
 *
 * @param[in] physicalDevice
 * Physical device.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] memoryRequirementCount
 * Memory requirement count.
 *
 * @param[in] pMemoryRequirements
 * Memory requirements.
 *
 * @param[in] pMemoryPropertyFlags
 * Memory property flags per requirement.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[out] pSharedMemory
 * Shared memory.
 *
 * @pre
 * - `physicalDevice` is valid
 * - `device` is valid
 * - `pMemoryRequirements` points to `memoryRequirementCount` values
 * - `pMemoryPropertyFlags` points to `memoryRequirementCount` values
 * - `pSharedMemory` is non-`NULL`
 * - `pSharedMemory` is uninitialized
 *
 * @post
 * - on success, `pSharedMemory` is properly initialized
 * - on failure, `pSharedMemory` is nullified
 *
 * @note
 * If `memoryRequirementCount` is `0`,
 * `pMemoryRequirements` may be `NULL`,
 * `pMemoryPropertyFlags` may be `NULL`,
 * `pSharedMemory` is nullified, and result is `VK_SUCCESS`.
 */
VkResult vkxAllocateSharedMemory(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            uint32_t memoryRequirementCount,
            const VkMemoryRequirements* pMemoryRequirements,
            const VkMemoryPropertyFlags* pMemoryPropertyFlags,
            const VkAllocationCallbacks* pAllocator,
            VkxSharedDeviceMemory* pSharedMemory);

/**
 * @brief Free shared memory.
 *
 * @param[in] device
 * Device.
 *
 * @param[inout] pSharedMemory
 * Shared memory.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @pre
 * - `device` was used to allocate `pSharedMemory`
 * - `pAllocator` was used to allocate `pSharedMemory`
 * - `pSharedMemory` was previously allocated by `vkxAllocateSharedMemory`
 *
 * @post
 * - `pSharedMemory` is nullified
 *
 * @note
 * Does nothing if `pSharedMemory` is `NULL`.
 */
void vkxFreeSharedMemory(
            VkDevice device,
            VkxSharedDeviceMemory* pSharedMemory,
            const VkAllocationCallbacks* pAllocator);

/**@}*/

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef VULKANX_MEMORY_H
