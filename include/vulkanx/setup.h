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
#ifndef VULKANX_SETUP_H
#define VULKANX_SETUP_H

#include <vulkan/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/**
 * @defgroup setup Setup
 *
 * `<vulkanx/setup.h>`
 */
/**@{*/

/**
 * @brief Instance create info.
 */
typedef struct VkxInstanceCreateInfo_
{
    /** @brief Application name. */
    const char* pApplicationName;

    /** @brief Application version, from `VK_MAKE_VERSION`. */
    uint32_t applicationVersion;

    /** @brief Engine name. */
    const char* pEngineName;

    /** @brief Engine version, from `VK_MAKE_VERSION`. */
    uint32_t engineVersion;

    /** @brief API version. */
    uint32_t apiVersion;

    /** @brief Requested instance layer count. */
    uint32_t requestedLayerCount;

    /** @brief Requested instance layer names. */
    const char* const* ppRequestedLayerNames;

    /** @brief Requested instance extension count. */
    uint32_t requestedExtensionCount;

    /** @brief Requested instance extension names. */
    const char* const* ppRequestedExtensionNames;
}
VkxInstanceCreateInfo;

/**
 * @brief Create instance.
 *
 * @param[in] pCreateInfo
 * Create info.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[out] pRequestedLayersEnabled
 * _Optional_. Booleans to indicate which requested extensions were enabled.
 *
 * @param[out] pRequestedExtensionsEnabled
 * _Optional_. Booleans to indicate which requested layers were enabled.
 *
 * @param[out] pInstance
 * Instance.
 */
VkResult vkxCreateInstance(
            const VkxInstanceCreateInfo* pCreateInfo,
            const VkAllocationCallbacks* pAllocator,
            VkBool32* pRequestedLayersEnabled,
            VkBool32* pRequestedExtensionsEnabled,
            VkInstance* pInstance);

/**
 * @brief Physical device select info.
 */
typedef struct VkxPhysicalDeviceSelectInfo_
{
    /** @brief _Optional_. Requested name. */
    const char* pRequestedName;

    /** @brief _Optional_. Requested features. */
    const VkPhysicalDeviceFeatures* pRequestedFeatures;

    /** @brief _Optional_. Is physical device okay for selection? */
    VkBool32 (*pIsPhysicalDeviceOkay)(VkPhysicalDevice, void*);

    /** @brief _Optional_. User data for callback. */
    void* pUserData;
}
VkxPhysicalDeviceSelectInfo;

/**
 * @brief Select physical device.
 *
 * @param[in] instance
 * Instance.
 *
 * @param[in] pSelectInfo
 * _Optional_. Select info.
 */
VkPhysicalDevice vkxSelectPhysicalDevice(
            VkInstance instance,
            const VkxPhysicalDeviceSelectInfo* pSelectInfo);

/**
 * @brief Select format.
 *
 * @param[in] physicalDevice
 * Physical device.
 *
 * @param[in] requestedTiling
 * Requested tiling.
 *
 * @param[in] requestedFeatures
 * Requested features.
 *
 * @param[in] candidateFormatCount
 * Candidate format count.
 *
 * @param[in] pCandidateFormats
 * Candidate formats.
 *
 * @pre
 * - `physicalDevice` is valid
 * - `pCandidateFormats` points to `candidateFormatCount` values
 *
 * @return
 * Format in `pCandidateFormats` with `requestedFeatures`, or
 * `VK_FORMAT_UNDEFINED` if no such format exists.
 */
VkFormat vkxSelectFormat(
            VkPhysicalDevice physicalDevice,
            VkImageTiling requestedTiling,
            VkFormatFeatureFlags requestedFeatures,
            uint32_t candidateFormatCount,
            const VkFormat* pCandidateFormats);

/**
 * @brief Get supported depth format.
 *
 * @param[in] physicalDevice
 * Physical device.
 */
VkFormat vkxGetDepthFormat(VkPhysicalDevice physicalDevice);

/**
 * @brief Get supported depth/stencil format.
 *
 * @param[in] physicalDevice
 * Physical device.
 */
VkFormat vkxGetDepthStencilFormat(VkPhysicalDevice physicalDevice);

/**
 * @brief Get format texel size in bytes.
 *
 * @param[in] format
 * Format.
 *
 * @note
 * For the time being, the implementation handles uncompressed color
 * and depth/stencil formats only. If `format` is a compressed format, 
 * plane format, or extension format, the implementation returns `0`.
 */
uint32_t vkxGetFormatTexelSize(VkFormat format);

/**
 * @brief Device queue family.
 */
typedef struct VkxDeviceQueueFamily_
{
    /** @brief Queue flags. */
    VkQueueFlags queueFlags;

    /** @brief Queue family properties. */
    VkQueueFamilyProperties queueFamilyProperties;

    /** @brief Queue family index. */
    uint32_t queueFamilyIndex;

    /** @brief Queue count. */
    uint32_t queueCount;

    /** @brief Queues. */
    VkQueue* pQueues;

    /** @brief Queue priorities. */
    float* pQueuePriorities;

    /** @brief Command pool count. */
    uint32_t commandPoolCount;

    /** @brief Command pools. */
    VkCommandPool* pCommandPools;

    /** @brief Command pool flags for each command pool. */
    VkCommandPoolCreateFlags* pCommandPoolCreateFlags;
}
VkxDeviceQueueFamily;

/**
 * @brief Device queue family create info.
 */
typedef struct VkxDeviceQueueFamilyCreateInfo_
{
    /** @brief Queue flags. */
    VkQueueFlags queueFlags;

    /** @brief Queue count. */
    uint32_t queueCount;

    /** @brief Minimum queue count. */
    uint32_t minQueueCount;

    /** @brief If creating many queues, use equal or inequal priority? */
    VkBool32 useEqualPriority;

    /** @brief Surface for present operations, or `VK_NULL_HANDLE`. */
    VkSurfaceKHR presentSurface;

    /** @brief Command pool count. */
    uint32_t commandPoolCount;

    /** @brief Command pool create falgs. */
    const VkCommandPoolCreateFlags* pCommandPoolCreateFlags;
}
VkxDeviceQueueFamilyCreateInfo;

/**
 * @brief Device.
 */
typedef struct VkxDevice_
{
    /** @brief Physical device. */
    VkPhysicalDevice physicalDevice;

    /** @brief Physical device features. */
    VkPhysicalDeviceFeatures* pPhysicalDeviceFeatures;

    /** @brief Logical device. */
    VkDevice device;

    /** @brief Queue family count. */
    uint32_t queueFamilyCount;

    /** @brief Queue families. */
    VkxDeviceQueueFamily* pQueueFamilies;
}
VkxDevice;

/**
 * @brief Device create info.
 */
typedef struct VkxDeviceCreateInfo_
{
    /** @brief _Optional_. Physical device select info. */
    const VkxPhysicalDeviceSelectInfo* pSelectInfo;

    /** @brief Queue family create info count. */
    uint32_t queueFamilyCreateInfoCount;

    /** @brief Queue family create infos. */
    const VkxDeviceQueueFamilyCreateInfo* pQueueFamilyCreateInfos;

    /** @brief _Optional_. Enabled extension count. */
    uint32_t enabledExtensionCount;

    /** @brief _Optional_. Enabled extension names. */
    const char* const* ppEnabledExtensionNames;
}
VkxDeviceCreateInfo;

/**
 * @brief Create device.
 */
VkResult vkxCreateDevice(
            VkInstance instance,
            const VkxDeviceCreateInfo* pCreateInfo,
            const VkAllocationCallbacks* pAllocator,
            VkxDevice* pDevice);

/**
 * @brief Destroy device.
 */
void vkxDestroyDevice(
            VkxDevice* pDevice, 
            const VkAllocationCallbacks* pAllocator);

/**@}*/

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef VULKANX_SETUP_H
