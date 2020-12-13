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

    /** @brief Associated physical device. */
    VkPhysicalDevice physicalDevice;

    /** @brief Associated device. */
    VkDevice device;
 
    /** @brief Present mode. */
    VkPresentModeKHR presentMode;

    /** @brief Surface format. */
    VkSurfaceFormatKHR surfaceFormat;

    /** @brief Pre-transform. */
    VkSurfaceTransformFlagBitsKHR preTransform;

    /** @brief Composite alpha. */
    VkCompositeAlphaFlagBitsKHR compositeAlpha;

    /** @brief Image usage flags. */
    VkImageUsageFlags imageUsage;

    /** @brief Queue family index count. */
    uint32_t queueFamilyIndexCount;

    /** @brief Queue family indices. */
    uint32_t queueFamilyIndices[2];

    /** @brief Image sharing mode. */
    VkSharingMode imageSharingMode;

    /** @brief Graphics queue. */
    VkQueue graphicsQueue;

    /** @brief Present queue. */
    VkQueue presentQueue;


    /**@}*/

    /**
     * @name Swapchain
     */
    /**@{*/

    /** @brief Swapchain. */
    VkSwapchainKHR swapchain;

    /** @brief Image extent. */
    VkExtent2D imageExtent;

    /** @brief Image count. */
    uint32_t imageCount;

    /** @brief Images. */
    VkImage* pImages;

    /** @brief Image views. */
    VkImageView* pImageViews;

    /** @brief Image indices. */
    uint32_t* pIndices;

    /** @brief Acquired semaphores. */
    VkSemaphore* pAcquiredSemaphores;

    /** @brief Released semaphores. */
    VkSemaphore* pReleasedSemaphores;

    /** @brief Next acquired semaphore. */
    VkSemaphore nextAcquiredSemaphore;

    /** @brief Next released semaphore. */
    VkSemaphore nextReleasedSemaphore;

    /** @brief Fences. */
    VkFence* pFences;

    /** @brief Command pool. */
    VkCommandPool commandPool;

    /** @brief Command buffers. */
    VkCommandBuffer* pCommandBuffers;

    /**@}*/

    /**
     * @name Swapchain latent state
     */
    /**@{*/

    /** @brief Render pass. */
    VkRenderPass renderPass;

    /** @brief Framebuffers. */
    VkFramebuffer* pFramebuffers;

    /**@}*/

    /**
     * @name Swapchain active state
     */
    /**@{*/

    /** @brief Active image index. */
    uint32_t activeIndex;

    /** @brief Active acquried semaphore. */
    VkSemaphore activeAcquiredSemaphore;

    /** @brief Active released semaphore. */
    VkSemaphore activeReleasedSemaphore;

    /** @brief Active fence. */
    VkFence activeFence;

    /** @brief Active command buffer. */
    VkCommandBuffer activeCommandBuffer;

    /** @brief Active framebuffer. */
    VkFramebuffer activeFramebuffer;

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
 * - `pSwapchain` is non-`NULL`
 * - `pSwapchain` is uninitialized
 *
 * @post
 * - on success, `pSwapchain` is properly initialized
 * - on failure, `pSwapchain` is nullified
 */
VkResult vkxCreateSwapchain(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            uint32_t graphicsQueueFamilyIndex,
            uint32_t presentQueueFamilyIndex,
            VkSurfaceKHR surface,
            VkExtent2D surfaceExtent,
            const VkAllocationCallbacks* pAllocator,
            VkxSwapchain* pSwapchain);

/**
 * @brief Recreate swapchain.
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
 * - `surface` was used to create `pSwapchain`
 * - `pAllocator` was used to create `pSwapchain`
 * - `pSwapchain` is non-`NULL`
 * - `pSwapchain` was previously created by `vkxCreateSwapchain`
 */
VkResult vkxRecreateSwapchain(
            VkSurfaceKHR surface,
            VkExtent2D surfaceExtent,
            const VkAllocationCallbacks* pAllocator,
            VkxSwapchain* pSwapchain);

/**
 * @brief Destroy swapchain.
 *
 * @param[inout] pSwapchain
 * Swapchain.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @pre
 * - `pAllocator` was used to create `pSwapchain`
 * - `pSwapchain` was previously created by `vkxCreateSwapchain`
 *
 * @post
 * - `pSwapchain` is nullified
 * 
 * @note
 * Does nothing if `pSwapchain` is `NULL`.
 */
void vkxDestroySwapchain(
            VkxSwapchain* pSwapchain,
            const VkAllocationCallbacks* pAllocator);

/**
 * @brief Setup render pass.
 */
VkResult vkxSwapchainSetupRenderPass(
            VkxSwapchain* pSwapchain,
            const VkRenderPassCreateInfo* pCreateInfo,
            const VkAllocationCallbacks* pAllocator);

/**
 * @brief Acquire next image.
 *
 * @param[in] device
 * Device.
 *
 * @param[inout] pSwapchain
 * Swapchain.
 *
 * @param[in] timeout
 * Timeout.
 */
VkResult vkxSwapchainAcquireNextImage(
            VkxSwapchain* pSwapchain, uint64_t timeout);

VkResult vkxSwapchainSubmit(
            VkxSwapchain* pSwapchain);

VkResult vkxSwapchainPresent(
            VkxSwapchain* pSwapchain,
            uint32_t moreWaitSemaphoreCount,
            const VkSemaphore* pMoreWaitSemaphores);

/**@}*/

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef VULKANX_SWAPCHAIN_H
