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
 * @brief Create instance.
 *
 * @param[in] pApplicationName
 * Application name.
 *
 * @param[in] applicationVersion
 * Application version, from `VK_MAKE_VERSION`.
 *
 * @param[in] pEngineName
 * Engine name.
 *
 * @param[in] engineVersion
 * Engine version, from `VK_MAKE_VERSION`.
 *
 * @param[in] apiVersion
 * API version.
 *
 * @param[in] requestedLayerCount
 * Requested instance layer count.
 *
 * @param[in] ppRequestedLayerNames
 * Requested instance layer names. 
 *
 * @param[in] requestedExtensionCount
 * Requested instance extension count.
 *
 * @param[in] ppRequestedExtensionNames
 * Requested instance extension names.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[out] pRequestedLayersEnabled
 * _Optional_. Booleans to indicate which requested layers were enabled.
 *
 * @param[out] pRequestedExtensionsEnabled
 * _Optional_. Booleans to indicate which requested extensions were enabled.
 *
 * @param[out] pInstance
 * Instance.
 */
VkResult vkxCreateInstance(
            const char* pApplicationName, 
            uint32_t applicationVersion,
            const char* pEngineName,
            uint32_t engineVersion,
            uint32_t apiVersion,
            uint32_t requestedLayerCount, 
            const char* const* ppRequestedLayerNames,
            uint32_t requestedExtensionCount,
            const char* const* ppRequestedExtensionNames,
            const VkAllocationCallbacks* pAllocator,
            VkBool32* pRequestedLayersEnabled,
            VkBool32* pRequestedExtensionsEnabled,
            VkInstance* pInstance)
                __attribute__((nonnull(13)));

/**
 * @brief Select physical device.
 *
 * @param[in] instance
 * Instance.
 *
 * @param[in] pRequestedName
 * _Optional_. Requested device name.
 *
 * @param[in] pRequestedFeatures
 * _Optional_. Requested device features.
 *
 * @param[in] isPhysicalDeviceOkay
 * Is physical device okay?
 *
 * @param[in] pUserData
 * User data.
 */
VkPhysicalDevice vkxSelectPhysicalDevice(
            VkInstance instance,
            const char* pRequestedName,
            const VkPhysicalDeviceFeatures* pRequestedFeatures,
            VkBool32 (*isPhysicalDeviceOkay)(VkPhysicalDevice, void*),
            void* pUserData);

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

/**@}*/

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef VULKANX_SETUP_H
