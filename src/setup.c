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
#include <vulkanx/result.h>
#include <vulkanx/memory.h>
#include <vulkanx/setup.h>

// Create instance.
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
{
    assert(pInstance);

    // Allocate layer properties.
    uint32_t layerPropertyCount = 0;
    VkLayerProperties* pLayerProperties = NULL;
    vkEnumerateInstanceLayerProperties(&layerPropertyCount, NULL);
    pLayerProperties = 
        (VkLayerProperties*)VKX_LOCAL_MALLOC(
                sizeof(VkLayerProperties) * layerPropertyCount);
    {
        // Retrieve layer properties.
        VkResult result = 
            vkEnumerateInstanceLayerProperties(
                    &layerPropertyCount, 
                    pLayerProperties);
        if (VKX_IS_ERROR(result)) {
            // Free layer properties.
            VKX_LOCAL_FREE(pLayerProperties);
            return result;
        }
    }

    // Allocate extension properties.
    uint32_t extensionPropertyCount = 0;
    VkExtensionProperties* pExtensionProperties = NULL;
    vkEnumerateInstanceExtensionProperties(
            NULL, 
            &extensionPropertyCount, NULL);
    pExtensionProperties = 
        (VkExtensionProperties*)VKX_LOCAL_MALLOC(
                sizeof(VkExtensionProperties) * extensionPropertyCount);
    {
        // Retrieve extension properties.
        VkResult result = 
            vkEnumerateInstanceExtensionProperties(
                    NULL,
                    &extensionPropertyCount, 
                    pExtensionProperties);
        if (VKX_IS_ERROR(result)) {
            // Free layer properties.
            VKX_LOCAL_FREE(pLayerProperties);
            // Free extension properties.
            VKX_LOCAL_FREE(pExtensionProperties);
            return result;
        }
    }

    // Enabled layers.
    uint32_t enabledLayerCount = 0;
    const char** ppEnabledLayerNames = 
        (const char**)VKX_LOCAL_MALLOC(
                sizeof(const char*) * requestedLayerCount);

    // Enabled extensions.
    uint32_t enabledExtensionCount = 0;
    const char** ppEnabledExtensionNames = 
        (const char**)VKX_LOCAL_MALLOC(
                sizeof(const char*) * requestedExtensionCount);

    // Iterate requested layers.
    for (uint32_t requestedLayerIndex = 0;
                  requestedLayerIndex < requestedLayerCount;
                  requestedLayerIndex++) {
        const char* pRequestedLayerName = 
                    ppRequestedLayerNames[requestedLayerIndex];

        // Iterate layers.
        VkBool32 enabled = VK_FALSE;
        for (uint32_t layerIndex = 0;
                      layerIndex < layerPropertyCount;
                      layerIndex++) {
            const char* pLayerName = 
                        pLayerProperties[layerIndex].layerName;

            // Found?
            if (strcmp(pRequestedLayerName, pLayerName) == 0) {
                ppEnabledLayerNames[enabledLayerCount++] = pLayerName;
                enabled = VK_TRUE;
                break;
            }
        }
        if (pRequestedLayersEnabled) {
            pRequestedLayersEnabled[requestedLayerIndex] = enabled;
        }
    }

    // Iterate requested extensions.
    for (uint32_t requestedExtensionIndex = 0;
                  requestedExtensionIndex < requestedExtensionCount;
                  requestedExtensionIndex++) {
        const char* pRequestedExtensionName = 
                    ppRequestedExtensionNames[requestedExtensionIndex];

        // Iterate extensions.
        VkBool32 enabled = VK_FALSE;
        for (uint32_t extensionIndex = 0;
                      extensionIndex < extensionPropertyCount;
                      extensionIndex++) {
            const char* pExtensionName = 
                        pExtensionProperties[extensionIndex].extensionName;

            // Found?
            if (strcmp(pRequestedExtensionName, pExtensionName) == 0) {
                ppEnabledExtensionNames[
                  enabledExtensionCount++] = pExtensionName;
                enabled = VK_TRUE;
                break;
            }
        }
        if (pRequestedExtensionsEnabled) {
            pRequestedExtensionsEnabled[requestedExtensionIndex] = enabled;
        }
    }

    // Create instance.
    VkApplicationInfo applicationInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = pApplicationName,
        .applicationVersion = applicationVersion,
        .pEngineName = pEngineName,
        .engineVersion = engineVersion,
        .apiVersion = apiVersion
    };
    VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = enabledLayerCount,
        .ppEnabledLayerNames = ppEnabledLayerNames,
        .enabledExtensionCount = enabledExtensionCount,
        .ppEnabledExtensionNames = ppEnabledExtensionNames
    };
    VkResult result = 
        vkCreateInstance(
                &instanceCreateInfo, pAllocator,
                pInstance);

    // Free enabled extension names.
    VKX_LOCAL_FREE(ppEnabledExtensionNames);

    // Free enabled layer names.
    VKX_LOCAL_FREE(ppEnabledLayerNames);

    // Free extension properties.
    VKX_LOCAL_FREE(pExtensionProperties);

    // Free layer properties.
    VKX_LOCAL_FREE(pLayerProperties);

    return result;
}

// Is physical device less?
static VkBool32 isPhysicalDeviceLess(
                VkPhysicalDevice physicalDevice1,
                VkPhysicalDevice physicalDevice2,
                const VkPhysicalDeviceFeatures* pRequestedFeatures)
{
    if (physicalDevice1 == VK_NULL_HANDLE) {
        return VK_TRUE;
    }
    if (physicalDevice2 == VK_NULL_HANDLE) {
        return VK_FALSE;
    }

    // Compare device types.
    VkPhysicalDeviceProperties properties1;
    VkPhysicalDeviceProperties properties2;
    vkGetPhysicalDeviceProperties(physicalDevice1, &properties1);
    vkGetPhysicalDeviceProperties(physicalDevice2, &properties2);
    if (properties1.deviceType !=
        properties2.deviceType) {

        // Prefer discrete GPUs first.
        if (properties1.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            return VK_FALSE;
        }
        if (properties2.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            return VK_TRUE;
        }

        // Prefer integrated GPUs second.
        if (properties1.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
            return VK_FALSE;
        }
        if (properties2.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
            return VK_TRUE;
        }
    }

    int score1 = 0;
    int score2 = 0;

    if (pRequestedFeatures) {

        // Score features.
        VkPhysicalDeviceFeatures features1;
        VkPhysicalDeviceFeatures features2;
        vkGetPhysicalDeviceFeatures(physicalDevice1, &features1);
        vkGetPhysicalDeviceFeatures(physicalDevice2, &features2);
        for (size_t pos = 0;
                    pos < sizeof(VkPhysicalDeviceFeatures) /
                          sizeof(VkBool32); pos++) {
            if (((const VkBool32*)pRequestedFeatures)[pos] == VK_TRUE) {
                score1 += ((const VkBool32*)&features1)[pos] == VK_TRUE;
                score2 += ((const VkBool32*)&features2)[pos] == VK_TRUE;
            }
        }
        if (score1 < score2) {
            return VK_TRUE;
        }
        if (score1 > score2) {
            return VK_FALSE;
        }
    }

    // Score limits.
    VkPhysicalDeviceLimits limits1 = properties1.limits;
    VkPhysicalDeviceLimits limits2 = properties2.limits;

    // Define helpers.
    #define FAVOR_MIN_LIMIT(LIMIT)\
            score1 += (limits1.LIMIT < limits2.LIMIT);\
            score2 += (limits1.LIMIT > limits2.LIMIT)
    #define FAVOR_MAX_LIMIT(LIMIT)\
            score1 += (limits1.LIMIT > limits2.LIMIT);\
            score2 += (limits1.LIMIT < limits2.LIMIT)
    {
        FAVOR_MAX_LIMIT(maxImageDimension1D);
        FAVOR_MAX_LIMIT(maxImageDimension2D);
        FAVOR_MAX_LIMIT(maxImageDimension3D);
        FAVOR_MAX_LIMIT(maxImageDimensionCube);
        FAVOR_MAX_LIMIT(maxImageArrayLayers);
        FAVOR_MAX_LIMIT(maxTexelBufferElements);
        FAVOR_MAX_LIMIT(maxUniformBufferRange);
        FAVOR_MAX_LIMIT(maxPushConstantsSize);
        FAVOR_MAX_LIMIT(maxMemoryAllocationCount);
        FAVOR_MAX_LIMIT(maxSamplerAllocationCount);
        FAVOR_MIN_LIMIT(bufferImageGranularity);
        FAVOR_MAX_LIMIT(sparseAddressSpaceSize);
        FAVOR_MAX_LIMIT(maxBoundDescriptorSets);
        FAVOR_MAX_LIMIT(maxPerStageDescriptorSamplers);
        FAVOR_MAX_LIMIT(maxPerStageDescriptorUniformBuffers);
        FAVOR_MAX_LIMIT(maxPerStageDescriptorStorageBuffers);
        FAVOR_MAX_LIMIT(maxPerStageDescriptorSampledImages);
        FAVOR_MAX_LIMIT(maxPerStageDescriptorStorageImages);
        FAVOR_MAX_LIMIT(maxPerStageDescriptorInputAttachments);
        FAVOR_MAX_LIMIT(maxPerStageResources);
        FAVOR_MAX_LIMIT(maxDescriptorSetSamplers);
        FAVOR_MAX_LIMIT(maxDescriptorSetUniformBuffers);
        FAVOR_MAX_LIMIT(maxDescriptorSetUniformBuffersDynamic);
        FAVOR_MAX_LIMIT(maxDescriptorSetStorageBuffers);
        FAVOR_MAX_LIMIT(maxDescriptorSetStorageBuffersDynamic);
        FAVOR_MAX_LIMIT(maxDescriptorSetSampledImages);
        FAVOR_MAX_LIMIT(maxDescriptorSetStorageImages);
        FAVOR_MAX_LIMIT(maxDescriptorSetInputAttachments);
        FAVOR_MAX_LIMIT(maxVertexInputAttributes);
        FAVOR_MAX_LIMIT(maxVertexInputBindings);
        FAVOR_MAX_LIMIT(maxVertexInputAttributeOffset);
        FAVOR_MAX_LIMIT(maxVertexInputBindingStride);
        FAVOR_MAX_LIMIT(maxVertexOutputComponents);
        FAVOR_MAX_LIMIT(maxTessellationGenerationLevel);
        FAVOR_MAX_LIMIT(maxTessellationPatchSize);
        FAVOR_MAX_LIMIT(maxTessellationControlPerVertexInputComponents);
        FAVOR_MAX_LIMIT(maxTessellationControlPerVertexOutputComponents);
        FAVOR_MAX_LIMIT(maxTessellationControlPerPatchOutputComponents);
        FAVOR_MAX_LIMIT(maxTessellationControlTotalOutputComponents);
        FAVOR_MAX_LIMIT(maxTessellationEvaluationInputComponents);
        FAVOR_MAX_LIMIT(maxTessellationEvaluationOutputComponents);
        FAVOR_MAX_LIMIT(maxGeometryShaderInvocations);
        FAVOR_MAX_LIMIT(maxGeometryInputComponents);
        FAVOR_MAX_LIMIT(maxGeometryOutputComponents);
        FAVOR_MAX_LIMIT(maxGeometryOutputVertices);
        FAVOR_MAX_LIMIT(maxGeometryTotalOutputComponents);
        FAVOR_MAX_LIMIT(maxFragmentInputComponents);
        FAVOR_MAX_LIMIT(maxFragmentOutputAttachments);
        FAVOR_MAX_LIMIT(maxFragmentDualSrcAttachments);
        FAVOR_MAX_LIMIT(maxFragmentCombinedOutputResources);
        FAVOR_MAX_LIMIT(maxComputeSharedMemorySize);
        FAVOR_MAX_LIMIT(maxComputeWorkGroupCount[0]);
        FAVOR_MAX_LIMIT(maxComputeWorkGroupCount[1]);
        FAVOR_MAX_LIMIT(maxComputeWorkGroupCount[2]);
        FAVOR_MAX_LIMIT(maxComputeWorkGroupInvocations);
        FAVOR_MAX_LIMIT(maxComputeWorkGroupSize[0]);
        FAVOR_MAX_LIMIT(maxComputeWorkGroupSize[1]);
        FAVOR_MAX_LIMIT(maxComputeWorkGroupSize[2]);
        FAVOR_MAX_LIMIT(subPixelPrecisionBits);
        FAVOR_MAX_LIMIT(subTexelPrecisionBits);
        FAVOR_MAX_LIMIT(mipmapPrecisionBits);
        FAVOR_MAX_LIMIT(maxDrawIndexedIndexValue);
        FAVOR_MAX_LIMIT(maxDrawIndirectCount);
        FAVOR_MAX_LIMIT(maxSamplerLodBias);
        FAVOR_MAX_LIMIT(maxSamplerAnisotropy);
        FAVOR_MAX_LIMIT(maxViewports);
        FAVOR_MAX_LIMIT(maxViewportDimensions[0]);
        FAVOR_MAX_LIMIT(maxViewportDimensions[1]);
        FAVOR_MAX_LIMIT(viewportBoundsRange[0]);
        FAVOR_MAX_LIMIT(viewportBoundsRange[1]);
        FAVOR_MAX_LIMIT(viewportSubPixelBits);
        FAVOR_MIN_LIMIT(minMemoryMapAlignment);
        FAVOR_MIN_LIMIT(minTexelBufferOffsetAlignment);
        FAVOR_MIN_LIMIT(minUniformBufferOffsetAlignment);
        FAVOR_MIN_LIMIT(minStorageBufferOffsetAlignment);
        FAVOR_MIN_LIMIT(minTexelOffset);
        FAVOR_MAX_LIMIT(maxTexelOffset);
        FAVOR_MIN_LIMIT(minTexelGatherOffset);
        FAVOR_MAX_LIMIT(maxTexelGatherOffset);
        FAVOR_MIN_LIMIT(minInterpolationOffset);
        FAVOR_MAX_LIMIT(maxInterpolationOffset);
        FAVOR_MAX_LIMIT(subPixelInterpolationOffsetBits);
        FAVOR_MAX_LIMIT(maxFramebufferWidth);
        FAVOR_MAX_LIMIT(maxFramebufferHeight);
        FAVOR_MAX_LIMIT(maxFramebufferLayers);
        FAVOR_MAX_LIMIT(maxColorAttachments);
        FAVOR_MAX_LIMIT(maxSampleMaskWords);
        FAVOR_MAX_LIMIT(maxClipDistances);
        FAVOR_MAX_LIMIT(maxCullDistances);
        FAVOR_MAX_LIMIT(maxCombinedClipAndCullDistances);
        FAVOR_MAX_LIMIT(discreteQueuePriorities);
        FAVOR_MIN_LIMIT(pointSizeRange[0]);
        FAVOR_MAX_LIMIT(pointSizeRange[1]);
        FAVOR_MIN_LIMIT(lineWidthRange[0]);
        FAVOR_MAX_LIMIT(lineWidthRange[1]);
        FAVOR_MIN_LIMIT(pointSizeGranularity);
        FAVOR_MIN_LIMIT(lineWidthGranularity);
    }
    // Undef helpers.
    #undef FAVOR_MIN_LIMIT
    #undef FAVOR_MAX_LIMIT 

    return score1 < score2 ? VK_TRUE : VK_FALSE;
}

// Select physical device.
VkPhysicalDevice vkxSelectPhysicalDevice(
            VkInstance instance,
            const char* pRequestedName,
            const VkPhysicalDeviceFeatures* pRequestedFeatures,
            VkBool32 (*isPhysicalDeviceOkay)(VkPhysicalDevice, void*),
            void* pUserData)
{
    // Allocate physical devices.
    uint32_t physicalDeviceCount = 0;
    VkPhysicalDevice* pPhysicalDevices = NULL;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);
    pPhysicalDevices = 
        (VkPhysicalDevice*)VKX_LOCAL_MALLOC(
                sizeof(VkPhysicalDevice) * physicalDeviceCount);
    {
        // Retrieve physical devices.
        VkResult result = 
            vkEnumeratePhysicalDevices(
                    instance, 
                    &physicalDeviceCount, 
                    pPhysicalDevices);
        if (VKX_IS_ERROR(result)) {
            // Free physical devices.
            VKX_LOCAL_FREE(pPhysicalDevices);
            return VK_NULL_HANDLE;
        }
    }

    // Best physical device.
    VkPhysicalDevice bestPhysicalDevice = VK_NULL_HANDLE;

    // User supplied requested name?
    if (pRequestedName) {

        // Iterate.
        VkPhysicalDeviceProperties properties;
        for (uint32_t index = 0;
                      index < physicalDeviceCount; index++) {
            VkPhysicalDevice thisPhysicalDevice = pPhysicalDevices[index];
            vkGetPhysicalDeviceProperties(thisPhysicalDevice, &properties);
            if (strcmp(properties.deviceName, pRequestedName) == 0) {

                // Okay?
                if (isPhysicalDeviceOkay(thisPhysicalDevice, pUserData)) {
                    bestPhysicalDevice = thisPhysicalDevice;
                }
            }
        }
    }
    else {

        // Iterate.
        for (uint32_t index = 0;
                      index < physicalDeviceCount; index++) {
            // Update best device if better device is found.
            VkPhysicalDevice thisPhysicalDevice = pPhysicalDevices[index];
            if (isPhysicalDeviceOkay(thisPhysicalDevice, pUserData) &&
                isPhysicalDeviceLess(bestPhysicalDevice, thisPhysicalDevice, 
                    pRequestedFeatures)) {
                bestPhysicalDevice = thisPhysicalDevice;
            }
        }
    }

    // Free physical devices.
    VKX_LOCAL_FREE(pPhysicalDevices);

    // Done.
    return bestPhysicalDevice;
}

// Select format.
VkFormat vkxSelectFormat(
            VkPhysicalDevice physicalDevice,
            VkImageTiling requestedTiling,
            VkFormatFeatureFlags requestedFeatures,
            uint32_t candidateFormatCount,
            const VkFormat* pCandidateFormats)
{
    // Iterate.
    VkFormatProperties formatProperties;
    for (uint32_t index = 0;
                  index < candidateFormatCount; index++) {
        VkFormat format = pCandidateFormats[index];
        vkGetPhysicalDeviceFormatProperties(
                physicalDevice, format, &formatProperties);

        // Features.
        VkFormatFeatureFlags features = 0;
        switch (requestedTiling) {
            case VK_IMAGE_TILING_LINEAR:
                features = formatProperties.linearTilingFeatures;
                break;
            case VK_IMAGE_TILING_OPTIMAL:
                features = formatProperties.optimalTilingFeatures;
                break;
            default:
                break;
        }

        // Features suitable?
        if ((requestedFeatures & features) ==
             requestedFeatures) {
            // Found.
            return format;
        }
    }

    // Not found.
    return VK_FORMAT_UNDEFINED;
}

// Get supported depth format.
VkFormat vkxGetDepthFormat(VkPhysicalDevice physicalDevice)
{
    VkFormat candidateFormats[6] = {
        VK_FORMAT_D16_UNORM,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_X8_D24_UNORM_PACK32,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT
    };
    return vkxSelectFormat(physicalDevice, 0, 0, 6, &candidateFormats[0]);

}

// Get supported depth/stencil format.
VkFormat vkxGetDepthStencilFormat(VkPhysicalDevice physicalDevice)
{
    VkFormat candidateFormats[3] = {
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D32_SFLOAT_S8_UINT
    };
    return vkxSelectFormat(physicalDevice, 0, 0, 3, &candidateFormats[0]);
}

// Get format texel size.
uint32_t vkxGetFormatTexelSize(VkFormat format)
{
    // 8-bit format?
    if ((format == VK_FORMAT_R4G4_UNORM_PACK8) ||
        (format >= VK_FORMAT_R8_UNORM &&
         format <= VK_FORMAT_R8_SRGB) ||
        (format == VK_FORMAT_S8_UINT)) {
        return 1;
    }

    // 16-bit format?
    if ((format >= VK_FORMAT_R4G4B4A4_UNORM_PACK16 &&
         format <= VK_FORMAT_A1R5G5B5_UNORM_PACK16) ||
        (format >= VK_FORMAT_R8G8_UNORM &&
         format <= VK_FORMAT_R8G8_SRGB) ||
        (format >= VK_FORMAT_R16_UNORM &&
         format <= VK_FORMAT_R16_SFLOAT) ||
        (format == VK_FORMAT_R10X6_UNORM_PACK16) ||
        (format == VK_FORMAT_R12X4_UNORM_PACK16) ||
        (format == VK_FORMAT_D16_UNORM)) {
        return 2;
    }

    // 24-bit format?
    if ((format >= VK_FORMAT_R8G8B8_UNORM &&
         format <= VK_FORMAT_B8G8R8_SRGB) ||
        (format == VK_FORMAT_D16_UNORM_S8_UINT)) {
        return 3;
    }

    // 32-bit format?
    if ((format >= VK_FORMAT_R8G8B8A8_UNORM &&
         format <= VK_FORMAT_A2B10G10R10_SINT_PACK32) ||
        (format >= VK_FORMAT_R16G16_UNORM &&
         format <= VK_FORMAT_R16G16_SFLOAT) ||
        (format >= VK_FORMAT_R32_UINT &&
         format <= VK_FORMAT_R32_SFLOAT) ||
        (format == VK_FORMAT_B10G11R11_UFLOAT_PACK32) ||
        (format == VK_FORMAT_E5B9G9R9_UFLOAT_PACK32) ||
        (format == VK_FORMAT_R10X6G10X6_UNORM_2PACK16) ||
        (format == VK_FORMAT_R12X4G12X4_UNORM_2PACK16) ||
        (format == VK_FORMAT_G8B8G8R8_422_UNORM) ||
        (format == VK_FORMAT_B8G8R8G8_422_UNORM) ||
        (format == VK_FORMAT_X8_D24_UNORM_PACK32) ||
        (format == VK_FORMAT_D32_SFLOAT) ||
        (format == VK_FORMAT_D24_UNORM_S8_UINT)) {
        return 4;
    }

    // 40-bit format?
    if (format == VK_FORMAT_D32_SFLOAT_S8_UINT) {
        return 5;
    }

    // 48-bit format?
    if (format >= VK_FORMAT_R16G16B16_UNORM &&
        format <= VK_FORMAT_R16G16B16_SFLOAT) {
        return 6;
    }

    // 64-bit format?
    if ((format >= VK_FORMAT_R16G16B16A16_UNORM &&
         format <= VK_FORMAT_R16G16B16A16_SFLOAT) ||
        (format >= VK_FORMAT_R32G32_UINT &&
         format <= VK_FORMAT_R32G32_SFLOAT) ||
        (format >= VK_FORMAT_R64_UINT &&
         format <= VK_FORMAT_R64_SFLOAT) ||
        (format == VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16) ||
        (format == VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16) ||
        (format == VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16) ||
        (format == VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16) ||
        (format == VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16) ||
        (format == VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16) ||
        (format == VK_FORMAT_G16B16G16R16_422_UNORM) ||
        (format == VK_FORMAT_B16G16R16G16_422_UNORM)) {
        return 8;
    }

    // 96-bit format?
    if (format >= VK_FORMAT_R32G32B32_UINT &&
        format <= VK_FORMAT_R32G32B32_SFLOAT) {
        return 12;
    }

    // 128-bit format?
    if ((format >= VK_FORMAT_R32G32B32A32_UINT &&
         format <= VK_FORMAT_R32G32B32A32_SFLOAT) ||
        (format >= VK_FORMAT_R64G64_UINT &&
         format <= VK_FORMAT_R64G64_SFLOAT)) {
        return 16;
    }

    // TODO Handle compressed formats?

    // TODO Handle plane formats?

    return 0;
}
