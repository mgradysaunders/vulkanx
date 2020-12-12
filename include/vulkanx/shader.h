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
#ifndef VULKANX_SHADER_H
#define VULKANX_SHADER_H

#include <vulkan/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/**
 * @defgroup shader Shader
 *
 * `<vulkanx/shader.h>`
 */
/**@{*/

/**
 * @brief Shader module create info.
 */
typedef struct VkxShaderModuleCreateInfo_
{
    /** @brief Shader stage. */
    VkShaderStageFlagBits stage;

    /** @brief Code size. */
    size_t codeSize;

    /** @brief Code. */
    const uint32_t* pCode;

    /** @brief Filename to load code from, if `pCode` is `NULL`. */
    const char* pCodeFilename;
}
VkxShaderModuleCreateInfo;

/**
 * @brief Shader module group.
 */
typedef struct VkxShaderModuleGroup_
{
    /** @brief Module count. */
    uint32_t moduleCount;

    /** @brief Modules. */
    VkShaderModule* pModules;

    /** @brief Pipeline stage create infos, for convenience. */
    VkPipelineShaderStageCreateInfo* pStageCreateInfos;
}
VkxShaderModuleGroup;

/**
 * @brief Create shader module group.
 */
VkResult vkxCreateShaderModuleGroup(
            VkDevice device,
            uint32_t createInfoCount,
            const VkxShaderModuleCreateInfo* pCreateInfos,
            const VkAllocationCallbacks* pAllocator,
            VkxShaderModuleGroup* pGroup);

/**
 * @brief Destroy shader module group.
 */
void vkxDestroyShaderModuleGroup(
            VkDevice device,
            VkxShaderModuleGroup* pGroup,
            const VkAllocationCallbacks* pAllocator);

/**@}*/

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef VULKANX_SHADER_H
