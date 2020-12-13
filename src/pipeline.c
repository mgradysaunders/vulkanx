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
#include <vulkanx/pipeline.h>

static void* deepCopy(const void* pSrc, size_t size)
{
    void* pDst = malloc(size);
    memcpy(pDst, pSrc, size);
    return pDst;
}

static VkGraphicsPipelineCreateInfo convertCreateInfo(
            const VkxGraphicsPipelineCreateInfo* pCreateInfo)
{
    assert(pCreateInfo);
    assert(pCreateInfo->pInputState);

    // Create info.
    VkGraphicsPipelineCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stageCount = pCreateInfo->stageCount,
        .pStages = pCreateInfo->pStages,
        .pVertexInputState = NULL,
        .pInputAssemblyState = NULL,
        .pTessellationState = NULL,
        .pViewportState = NULL,
        .pRasterizationState = NULL,
        .pMultisampleState = NULL,
        .pDepthStencilState = NULL,
        .pColorBlendState = NULL,
        .pDynamicState = NULL,
        .layout = pCreateInfo->layout,
        .renderPass = pCreateInfo->renderPass,
        .subpass = pCreateInfo->subpass,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = pCreateInfo->basePipeline
    };

    // Vertex input state.
    VkPipelineVertexInputStateCreateInfo vertexInputState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .vertexBindingDescriptionCount = 
            pCreateInfo->pInputState->bindingCount,
        .pVertexBindingDescriptions =
            pCreateInfo->pInputState->pBindings,
        .vertexAttributeDescriptionCount =
            pCreateInfo->pInputState->attributeCount,
        .pVertexAttributeDescriptions =
            pCreateInfo->pInputState->pAttributes
    };
    createInfo.pVertexInputState =
            deepCopy(&vertexInputState, sizeof(vertexInputState));

    // Input assembly state.
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .topology = pCreateInfo->pInputState->topology,
        .primitiveRestartEnable = 
            pCreateInfo->pInputState->primitiveRestartEnable
    };
    createInfo.pInputAssemblyState =
            deepCopy(&inputAssemblyState, sizeof(inputAssemblyState));

    // Tessellation state.
    if (pCreateInfo->pInputState->patchControlPoints > 0) {
        VkPipelineTessellationStateCreateInfo tessellationState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .patchControlPoints = pCreateInfo->pInputState->patchControlPoints
        };
        createInfo.pTessellationState =
               deepCopy(&tessellationState, sizeof(tessellationState));
    }

    // Viewport state.
    if (pCreateInfo->viewportCount > 0) {
        VkRect2D* pScissors = malloc(
                sizeof(VkRect2D) * pCreateInfo->viewportCount);
        // Viewport scissors provided?
        if (pCreateInfo->pViewportScissors) {
            // Copy.
            memcpy(pScissors, 
                   pCreateInfo->pViewportScissors,
                   sizeof(VkRect2D) * pCreateInfo->viewportCount);
        }
        else {
            // Copy from viewports.
            for (uint32_t viewportIndex = 0; 
                          viewportIndex < pCreateInfo->viewportCount;
                          viewportIndex++) {
                const VkViewport* pViewport = 
                          pCreateInfo->pViewports + viewportIndex;
                VkRect2D* pScissor = pScissors + viewportIndex;
                pScissor->offset.x = pViewport->x;
                pScissor->offset.y = pViewport->y;
                pScissor->extent.width = pViewport->width;
                pScissor->extent.height = pViewport->height;
            }
        }
        VkPipelineViewportStateCreateInfo viewportState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .viewportCount = pCreateInfo->viewportCount,
            .pViewports = pCreateInfo->pViewports,
            .scissorCount = pCreateInfo->viewportCount,
            .pScissors = pScissors
        };
        createInfo.pViewportState =
                deepCopy(&viewportState, sizeof(viewportState));
    }

    // Rasterization state.
    VkPipelineRasterizationStateCreateInfo rasterizationState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .depthClampEnable = 
            pCreateInfo->pDepthState ? 
            pCreateInfo->pDepthState->depthClampEnable : VK_FALSE,
        .rasterizerDiscardEnable = 
            pCreateInfo->pInputState->rasterizerDiscardEnable,
        .polygonMode = 
            pCreateInfo->pInputState->polygonMode,
        .cullMode = 
            pCreateInfo->pInputState->cullMode,
        .frontFace = 
            pCreateInfo->pInputState->frontFace,
        .depthBiasEnable = 
            pCreateInfo->pDepthState ?
            pCreateInfo->pDepthState->depthBiasEnable : VK_FALSE,
        .depthBiasConstantFactor = 
            pCreateInfo->pDepthState ?
            pCreateInfo->pDepthState->depthBiasConstantFactor : 0.0f,
        .depthBiasClamp = 
            pCreateInfo->pDepthState ?
            pCreateInfo->pDepthState->depthBiasClamp : 0.0f,
        .depthBiasSlopeFactor = 
            pCreateInfo->pDepthState ?
            pCreateInfo->pDepthState->depthBiasSlopeFactor : 0.0f,
        .lineWidth = pCreateInfo->pInputState->lineWidth
    };
    createInfo.pRasterizationState =
            deepCopy(&rasterizationState, sizeof(rasterizationState));

    // Multisample state.
    if (pCreateInfo->pMultisampleState || 
        pCreateInfo->pInputState->rasterizerDiscardEnable == VK_FALSE) {
        VkPipelineMultisampleStateCreateInfo multisampleState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .rasterizationSamples = 
                pCreateInfo->pMultisampleState ?
                pCreateInfo->pMultisampleState->samples : 
                VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = 
                pCreateInfo->pMultisampleState ?
                pCreateInfo->pMultisampleState->sampleShadingEnable : 
                VK_FALSE,
            .minSampleShading = 
                pCreateInfo->pMultisampleState ?
                pCreateInfo->pMultisampleState->minSampleShading : 0.0f,
            .pSampleMask = 
                pCreateInfo->pMultisampleState &&
                pCreateInfo->pMultisampleState->sampleMaskEnable ?
                &pCreateInfo->pMultisampleState->sampleMask[0] : NULL,
            .alphaToCoverageEnable = 
                pCreateInfo->pMultisampleState ?
                pCreateInfo->pMultisampleState->alphaToCoverageEnable : 
                VK_FALSE,
            .alphaToOneEnable = 
                pCreateInfo->pMultisampleState ?
                pCreateInfo->pMultisampleState->alphaToOneEnable : VK_FALSE
        };
        createInfo.pMultisampleState =
                deepCopy(&multisampleState, sizeof(multisampleState));
    }

    // Depth/stencil state.
    if (pCreateInfo->pDepthState || pCreateInfo->pStencilState) {
        VkStencilOpState defaultStencilOp = {
            .failOp = (VkStencilOp)0,
            .passOp = (VkStencilOp)0,
            .depthFailOp = (VkStencilOp)0,
            .compareOp = (VkCompareOp)0,
            .compareMask = 0,
            .writeMask = 0,
            .reference = 0
        };
        VkPipelineDepthStencilStateCreateInfo depthStencilState = {
            .sType = 
                VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .depthTestEnable = pCreateInfo->pDepthState ? VK_TRUE : VK_FALSE,
            .depthWriteEnable = 
                pCreateInfo->pDepthState ?
                pCreateInfo->pDepthState->depthWriteEnable : VK_FALSE,
            .depthCompareOp = 
                pCreateInfo->pDepthState ?
                pCreateInfo->pDepthState->depthCompareOp : (VkCompareOp)0,
            .depthBoundsTestEnable =
                pCreateInfo->pDepthState ?
                pCreateInfo->pDepthState->depthBoundsTestEnable : VK_FALSE,
            .stencilTestEnable = 
                pCreateInfo->pStencilState ? VK_TRUE : VK_FALSE,
            .front = 
                pCreateInfo->pStencilState ?
                pCreateInfo->pStencilState->front : defaultStencilOp,
            .back = 
                pCreateInfo->pStencilState ?
                pCreateInfo->pStencilState->back : defaultStencilOp,
            .minDepthBounds = 
                pCreateInfo->pDepthState ?
                pCreateInfo->pDepthState->minDepthBounds : 0.0f,
            .maxDepthBounds = 
                pCreateInfo->pDepthState ?
                pCreateInfo->pDepthState->maxDepthBounds : 0.0f
        };
        createInfo.pDepthStencilState =
                deepCopy(&depthStencilState, sizeof(depthStencilState));
    }

    // Color blend state.
    VkPipelineColorBlendStateCreateInfo colorBlendState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .logicOpEnable = pCreateInfo->logicOpEnable,
        .logicOp = pCreateInfo->logicOp,
        .attachmentCount = pCreateInfo->blendAttachmentCount,
        .pAttachments = pCreateInfo->pBlendAttachments,
        .blendConstants = {
            pCreateInfo->blendConstants[0],
            pCreateInfo->blendConstants[1],
            pCreateInfo->blendConstants[2],
            pCreateInfo->blendConstants[3]
        }
    };
    createInfo.pColorBlendState =
            deepCopy(&colorBlendState, sizeof(colorBlendState));

    // Dynamic state.
    if (pCreateInfo->dynamicStateCount > 0) {
        VkPipelineDynamicStateCreateInfo dynamicState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .dynamicStateCount = pCreateInfo->dynamicStateCount,
            .pDynamicStates = pCreateInfo->pDynamicStates
        };
        createInfo.pDynamicState =
                deepCopy(&dynamicState, sizeof(dynamicState));
    }

    return createInfo;
}

VkResult vkxCreateGraphicsPipelines(
            VkDevice device,
            uint32_t createInfoCount,
            const VkxGraphicsPipelineCreateInfo* pCreateInfos,
            const VkAllocationCallbacks* pAllocator,
            VkPipeline* pPipelines)
{
    if (createInfoCount == 0) {
        return VK_SUCCESS;
    }
    assert(pCreateInfos);
    assert(pPipelines);

    // Allocate and initialize actual create infos.
    VkGraphicsPipelineCreateInfo* pActualCreateInfos = 
            VKX_LOCAL_MALLOC(
            sizeof(VkGraphicsPipelineCreateInfo) * createInfoCount);
    for (uint32_t createInfoIndex = 0; createInfoIndex < createInfoCount;
                  createInfoIndex++)
        pActualCreateInfos[createInfoIndex] = 
        convertCreateInfo(pCreateInfos + createInfoIndex);

    // If necessary, set derivative flags implicitly.
    for (uint32_t createInfoIndex = 0; createInfoIndex < createInfoCount;
                  createInfoIndex++) {
        VkGraphicsPipelineCreateInfo* 
            pActualCreateInfo = 
            pActualCreateInfos + createInfoIndex;
        if (pActualCreateInfo->basePipelineIndex >= 0) { 
            pActualCreateInfo->flags |= VK_PIPELINE_CREATE_DERIVATIVE_BIT;
            pActualCreateInfos[pActualCreateInfo->basePipelineIndex].flags |=
                VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
        }
    }

    // If more than 1 pipeline, use cache.
    VkPipelineCache pipelineCache = VK_NULL_HANDLE;
    if (createInfoCount > 1) {
        // Create cache.
        VkPipelineCacheCreateInfo cacheCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .initialDataSize = 0,
            .pInitialData = NULL
        };
        VkResult result = vkCreatePipelineCache(
                device, &cacheCreateInfo, pAllocator, &pipelineCache);

        // This should only fail if we're out of memory somewhere,
        // in which case we have bigger problems. Just in case it returns
        // some other error code, opt not to use the cache and proceed.
        if (result != VK_SUCCESS) {
            pipelineCache = VK_NULL_HANDLE;
        }
    }

    // Create graphics pipelines.
    VkResult result = vkCreateGraphicsPipelines(
            device,
            pipelineCache,
            createInfoCount,
            pActualCreateInfos,
            pAllocator,
            pPipelines);

    // Destroy cache.
    vkDestroyPipelineCache(device, pipelineCache, pAllocator);

    for (uint32_t createInfoIndex = 0; createInfoIndex < createInfoCount;
                  createInfoIndex++) {
        VkGraphicsPipelineCreateInfo* 
            pActualCreateInfo = 
            pActualCreateInfos + createInfoIndex;

        // Free temporary create info members.
        free((void*)pActualCreateInfo->pVertexInputState);
        free((void*)pActualCreateInfo->pInputAssemblyState);
        free((void*)pActualCreateInfo->pTessellationState);
        if (pActualCreateInfo->pViewportState) {
            free((void*)pActualCreateInfo->pViewportState->pScissors);
            free((void*)pActualCreateInfo->pViewportState);
        }
        free((void*)pActualCreateInfo->pRasterizationState);
        free((void*)pActualCreateInfo->pMultisampleState);
        free((void*)pActualCreateInfo->pDepthStencilState);
        free((void*)pActualCreateInfo->pColorBlendState);
        free((void*)pActualCreateInfo->pDynamicState);
    }

    // Free create infos.
    VKX_LOCAL_FREE(pActualCreateInfos);

    return result;
}
