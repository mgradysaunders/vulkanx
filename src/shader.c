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
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <vulkanx/shader.h>
#include <vulkanx/memory.h>

static uint32_t* readCodeFile(const char* pFilename, size_t* pCodeSize)
{
    // Open.
    FILE* pFile = fopen(pFilename, "rb");
    if (!pFile) {
        return NULL;
    }
    
    // Find size.
    fseek(pFile, 0, SEEK_END);
    *pCodeSize = ftell(pFile);
    if (*pCodeSize == 0 ||
        *pCodeSize % 4 != 0) {
        fclose(pFile);
        return NULL;
    }

    // Read.
    rewind(pFile);
    uint32_t* pCode = malloc(*pCodeSize);
    if (fread(pCode, 4, *pCodeSize / 4, pFile) != *pCodeSize) {
        free(pCode);
        fclose(pFile);
        return NULL;
    }
    fclose(pFile);
    return pCode;
}

VkResult vkxCreateShaderModuleGroup(
            VkDevice device,
            uint32_t createInfoCount,
            const VkxShaderModuleCreateInfo* pCreateInfos,
            const VkAllocationCallbacks* pAllocator,
            VkxShaderModuleGroup* pGroup)
{
    assert(pGroup);
    memset(pGroup, 0, sizeof(VkxShaderModuleGroup));

    // Allocate modules and stage create infos.
    pGroup->moduleCount = createInfoCount;
    pGroup->pModules = calloc(createInfoCount, sizeof(VkShaderModule));
    pGroup->pStageCreateInfos = calloc(createInfoCount, 
            sizeof(VkPipelineShaderStageCreateInfo));

    for (uint32_t createInfoIndex = 0; createInfoIndex < createInfoCount;
                  createInfoIndex++) {
        const VkxShaderModuleCreateInfo* 
              pCreateInfo = pCreateInfos + createInfoIndex;

        // Initialize shader module create info.
        VkShaderModuleCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .codeSize = pCreateInfo->codeSize,
            .pCode = pCreateInfo->pCode
        };

        // Read code from file?
        if (pCreateInfo->codeSize == 0) {
            // Read.
            createInfo.pCode = readCodeFile(
                pCreateInfo->pCodeFilename, &createInfo.codeSize);

            // Read failure?
            if (!createInfo.pCode) { 
                // Clean up.
                vkxDestroyShaderModuleGroup(device, pGroup, pAllocator);

                // Return error.
                return VK_ERROR_INITIALIZATION_FAILED;
            }
        }

        // Create.
        VkResult result = vkCreateShaderModule(
                device, 
                &createInfo, 
                pAllocator, 
                pGroup->pModules + createInfoIndex);

        // If code read from file, deallocate it now.
        if (pCreateInfo->codeSize == 0) {
            free((void*)createInfo.pCode);
        }

        // Failure?
        if (result != VK_SUCCESS) {
            // Clean up.
            vkxDestroyShaderModuleGroup(device, pGroup, pAllocator);

            // Return error.
            return result;
        }

        // Initialize stage create info.
        VkPipelineShaderStageCreateInfo stageCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .stage = pCreateInfo->stage,
            .module = pGroup->pModules[createInfoIndex],
            .pName = "main",
            .pSpecializationInfo = NULL
        };
        memcpy(pGroup->pStageCreateInfos + createInfoIndex,
               &stageCreateInfo, sizeof(stageCreateInfo));
    }

    return VK_SUCCESS;
}

void vkxDestroyShaderModuleGroup(
            VkDevice device,
            VkxShaderModuleGroup* pGroup,
            const VkAllocationCallbacks* pAllocator)
{
    if (pGroup) {
        // Destroy modules.
        for (uint32_t moduleIndex = 0; moduleIndex < pGroup->moduleCount;
                      moduleIndex++)
            vkDestroyShaderModule(
                    device, pGroup->pModules[moduleIndex], pAllocator);

        // Free modules.
        free(pGroup->pModules);

        // Free stage create infos.
        free(pGroup->pStageCreateInfos);

        // Nullify.
        memset(pGroup, 0, sizeof(VkxShaderModuleGroup));
    }
}
