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
#ifndef VULKANX_SWAPCHAIN_H
#define VULKANX_SWAPCHAIN_H

#include <vulkan/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/**
 * @defgroup swapchain Swapchain
 *
 * `<vulkanx/swapchain.h>`
 */
/**@{*/

/**
 * @brief Swapchain.
 */
typedef struct VkxSwapchain_
{
    /**
     * @name Swapchain configuration (immutable)
     */
    /**@{*/

    /**
     * @brief Present mode.
     */
    VkPresentModeKHR presentMode;

    /**
     * @brief Surface format.
     */
    VkSurfaceFormatKHR surfaceFormat;

    /**
     * @brief Pre-transform.
     */
    VkSurfaceTransformFlagBitsKHR preTransform;

    /**
     * @brief Composite alpha.
     */
    VkCompositeAlphaFlagBitsKHR compositeAlpha;

    /**
     * @brief Image usage flags.
     */
    VkImageUsageFlags imageUsage;

    /**
     * @brief Queue family index count.
     */
    uint32_t queueFamilyIndexCount;

    /**
     * @brief Queue family indices.
     */
    uint32_t queueFamilyIndices[2];

    /**
     * @brief Image sharing mode.
     */
    VkSharingMode imageSharingMode;

    /**@}*/

    /**
     * @name Swapchain
     */
    /**@{*/

    /**
     * @brief Swapchain.
     */
    VkSwapchainKHR swapchain;

    /**
     * @brief Image extent.
     */
    VkExtent2D imageExtent;

    /**
     * @brief Image count.
     */
    uint32_t imageCount;

    /**
     * @brief Images.
     */
    VkImage* pImages;

    /**
     * @brief Image views.
     */
    VkImageView* pImageViews;

    /**@}*/
}
VkxSwapchain;

/**
 * @brief Create swapchain.
 *
 * @param[in] physicalDevice
 * Physical device.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] graphicsQueueFamilyIndex
 * Graphics queue family index.
 *
 * @param[in] presentQueueFamilyIndex
 * Present queue family index.
 *
 * @param[in] surface
 * Surface.
 *
 * @param[in] surfaceExtent
 * Surface extent.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[out] pSwapchain
 * Swapchain.
 *
 * @pre
 * - `physicalDevice` is valid
 * - `device` is valid
 * - `graphicsQueueFamilyIndex` is valid and supports graphics commands
 * - `presentQueueFamilyIndex` is valid and supports present commands
 * - `surface` is valid
 * - `pSwapchain` is non-`NULL` and uninitialized
 */
VkResult vkxCreateSwapchain(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            uint32_t graphicsQueueFamilyIndex,
            uint32_t presentQueueFamilyIndex,
            VkSurfaceKHR surface,
            VkExtent2D surfaceExtent,
            const VkAllocationCallbacks* pAllocator,
            VkxSwapchain* pSwapchain)
                __attribute__((nonnull(8)));

/**
 * @brief Recreate swapchain.
 *
 * @param[in] physicalDevice
 * Physical device.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] surface
 * Surface.
 *
 * @param[in] surfaceExtent
 * Surface extent.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[inout] pSwapchain
 * Swapchain.
 *
 * @pre
 * - `physicalDevice` was used to create `pSwapchain`
 * - `device` was used to create `pSwapchain`
 * - `surface` was used to create `pSwapchain`
 * - `pAllocator` was used to create `pSwapchain`
 * - `pSwapchain` is non-`NULL`
 * - `pSwapchain` was previously created by `vkxCreateSwapchain`
 */
VkResult vkxRecreateSwapchain(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VkSurfaceKHR surface,
            VkExtent2D surfaceExtent,
            const VkAllocationCallbacks* pAllocator,
            VkxSwapchain* pSwapchain)
                __attribute__((nonnull(6)));

/**
 * @brief Destroy swapchain.
 *
 * @param[in] device
 * Device.
 *
 * @param[inout] pSwapchain
 * Swapchain.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @pre
 * - `device` was used to create `pSwapchain`
 * - `pAllocator` was used to create `pSwapchain`
 * - `pSwapchain` was previously created by `vkxCreateSwapchain`
 *
 * @post
 * - `pSwapchain` is nullified
 */
void vkxDestroySwapchain(
            VkDevice device,
            VkxSwapchain* pSwapchain,
            const VkAllocationCallbacks* pAllocator);

/**@}*/

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef VULKANX_SWAPCHAIN_H
