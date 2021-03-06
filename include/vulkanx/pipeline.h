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
#ifndef VULKANX_PIPELINE_H
#define VULKANX_PIPELINE_H

#include <vulkanx/memory.h>

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/**
 * @defgroup pipeline Pipeline
 *
 * `<vulkanx/pipeline.h>`
 */
/**@{*/

/**
 * @brief Graphics pipeline input state.
 *
 * This struct combines 
 * - `VkPipelineVertexInputStateCreateInfo`,
 * - `VkPipelineInputAssemblyStateCreateInfo`, 
 * - `VkPipelineTessellationStateCreateInfo`, and
 * - partially `VkPipelineRasterizationStateCreateInfo`. 
 *
 * This aims to capture the entirety of how graphics primitives are 
 * input to and rasterized by Vulkan. Hence, this includes the vertex
 * bindings and attributes, the primitive topology and culling options, 
 * and the number of patch control points if tessellation is used.
 */
typedef struct VkxGraphicsPipelineInputState_
{
    /** @brief Binding count. */
    uint32_t bindingCount;

    /** @brief Bindings. */
    const VkVertexInputBindingDescription* pBindings;

    /** @brief Attribute count. */
    uint32_t attributeCount;

    /** @brief Attributes. */
    const VkVertexInputAttributeDescription* pAttributes;

    /** @brief Topology. */
    VkPrimitiveTopology topology;

    /** @brief Front face setting. */
    VkFrontFace frontFace;

    /** @brief Cull mode. */
    VkCullModeFlags cullMode;

    /** @brief Polygon mode. */
    VkPolygonMode polygonMode;

    /** @brief Primitive restart enable? */
    VkBool32 primitiveRestartEnable;

    /** @brief Rasterizer discard enable? */
    VkBool32 rasterizerDiscardEnable;

    /** @brief Control points per patch, or 0 if no tessellation. */
    uint32_t patchControlPoints;

    /** @brief Line width, if rasterizing lines. */
    float lineWidth;
}
VkxGraphicsPipelineInputState;

/**
 * @brief Graphics pipeline depth state.
 *
 * This struct combines 
 * - partially `VkPipelineDepthStencilStateCreateInfo`, and
 * - partially `VkPipelineRasterizationStateCreateInfo`.
 */
typedef struct VkxGraphicsPipelineDepthState_
{
    /** @brief Depth clamp enable? */
    VkBool32 depthClampEnable;

    /** @brief Depth write enable? */
    VkBool32 depthWriteEnable;

    /** @brief Depth compare op. */
    VkCompareOp depthCompareOp;

    /** @brief Depth bounds test enable? */
    VkBool32 depthBoundsTestEnable;

    /** @brief Minimum depth bounds. */
    float minDepthBounds;

    /** @brief Maximum depth bounds. */
    float maxDepthBounds;

    /** @brief Depth bias enable? */
    VkBool32 depthBiasEnable;

    /** @brief Depth bias constant factor. */
    float depthBiasConstantFactor;

    /** @brief Depth bias clamp. */
    float depthBiasClamp;

    /** @brief Depth bias slope factor. */
    float depthBiasSlopeFactor;
}
VkxGraphicsPipelineDepthState;

/**
 * @brief Graphics pipeline stencil state.
 */
typedef struct VkxGraphicsPipelineStencilState_
{
    /** @brief Front. */
    VkStencilOpState front;

    /** @brief Back. */
    VkStencilOpState back;
}
VkxGraphicsPipelineStencilState;

/**
 * @brief Graphics pipeline multisample state.
 *
 * This struct wraps `VkPipelineMultisampleStateCreateInfo`. It is 
 * nearly identical, but inlines `pSampleMask` as `sampleMask` since it
 * should never be a pointer to more than two values. Also, this may be
 * left out of `VkxGraphicsPipelineCreateInfo`, in which case an effective
 * _no multisampling_ default struct is used.
 */
typedef struct VkxGraphicsPipelineMultisampleState_
{
    /** @brief Samples. */
    VkSampleCountFlagBits samples;

    /** @brief Sample shading enable? */
    VkBool32 sampleShadingEnable;

    /** @brief Minimum sample shading fraction. */
    float minSampleShading;

    /** @brief Sample mask enable? */
    VkBool32 sampleMaskEnable;

    /** @brief Sample mask. */
    VkSampleMask sampleMask[2];

    /** @brief Alpha-to-coverable enable? */
    VkBool32 alphaToCoverageEnable;

    /** @brief Alpha-to-one enable? */
    VkBool32 alphaToOneEnable;
}
VkxGraphicsPipelineMultisampleState;

/**
 * @brief Graphics pipeline create info.
 *
 * This struct replaces `VkGraphicsPipelineCreateInfo`. This uses 
 * - `VkxGraphicsPipelineInputState`, 
 * - `VkxGraphicsPipelineDepthState`,
 * - `VkxGraphicsPipelineStencilState`, and
 * - `VkxGraphicsPipelineMultisampleState`
 *
 * to replace 
 * - `VkPipelineVertexInputStateCreateInfo`,
 * - `VkPipelineInputAssemblyStateCreateInfo`,
 * - `VkPipelineTessellationStateCreateInfo`,
 * - `VkPipelineDepthStencilStateCreateInfo`,
 * - `VkPipelineRasterizationStateCreateInfo`, and
 * - `VkPipelineMultisampleStateCreateInfo`.
 *
 * This further inlines 
 * `VkPipelineViewportStateCreateInfo`,
 * `VkPipelineColorBlendStateCreateInfo`, and 
 * `VkPipelineDynamicStateCreateInfo`, thus reducing the number of 
 * intermediate structs from 9 to 4. 
 */
typedef struct VkxGraphicsPipelineCreateInfo_
{
    /** @brief Stage count. */
    uint32_t stageCount;

    /** @brief Stages. */
    const VkPipelineShaderStageCreateInfo* pStages;

    /** @brief Input state. */
    const VkxGraphicsPipelineInputState* pInputState;

    /** @brief _Optional_. Depth state. */
    const VkxGraphicsPipelineDepthState* pDepthState;

    /** @brief _Optional_. Stencil state. */
    const VkxGraphicsPipelineStencilState* pStencilState;

    /** @brief _Optional_. Multisample state. */
    const VkxGraphicsPipelineMultisampleState* pMultisampleState;

    /**
     * @name Viewport state
     *
     * Note that `VkPipelineViewportStateCreateInfo` has another field 
     * `scissorCount` to specify the number of scissors `pScissors`, which
     * is required to be equivalent to `viewportCount`. This struct makes 
     * this requirement implicit by removing `scissorCount` and renaming 
     * `pScissors` to `pViewportScissors`. 
     *
     * For convenience, `pViewportScissors` is optional. If `NULL`, the 
     * implementation initializes scissors to match each viewport in 
     * `pViewports`. 
     */
    /**@{*/

    /** @brief Viewport count. */
    uint32_t viewportCount;

    /** @brief Viewports. */
    const VkViewport* pViewports;

    /** @brief _Optional_. Viewport scissors. */
    const VkRect2D* pViewportScissors;

    /**@}*/

    /**
     * @name Color blend state
     */
    /**@{*/

    /** @brief Logic op enable? */
    VkBool32 logicOpEnable;

    /** @brief Logic op. */
    VkLogicOp logicOp;

    /** @brief Blend attachment count. */
    uint32_t blendAttachmentCount;

    /** @brief Blend attachments. */
    const VkPipelineColorBlendAttachmentState* pBlendAttachments;

    /** @brief Blend constants. */
    float blendConstants[4];

    /**@}*/

    /**
     * @name Dynamic state
     */
    /**@{*/

    /** @brief Dynamic state count. */
    uint32_t dynamicStateCount;

    /** @brief Dynamic states. */
    const VkDynamicState* pDynamicStates;

    /**@}*/

    /** @brief Layout. */
    VkPipelineLayout layout;

    /** @brief Render pass. */
    VkRenderPass renderPass;

    /** @brief Subpass index. */
    uint32_t subpass;

    /** @brief Base pipeline index, or -1 if no parent. */
    int32_t basePipeline;
}
VkxGraphicsPipelineCreateInfo;

/**
 * @brief Create graphics pipelines.
 *
 * @param[in] device
 * Device.
 *
 * @param[in] createInfoCount
 * Create info count.
 *
 * @param[in] pCreateInfos
 * Create infos.
 *
 * @param[in] pAllocator
 * _Optional_. Allocation callbacks.
 *
 * @param[out] pPipelines
 * Pipelines.
 *
 * @pre
 * - `device` is valid
 * - `pCreateInfos` points to `createInfoCount` values
 * - `pPipelines` points to `createInfoCount` values 
 * - `pPipelines` is `NULL` only if `createInfoCount` is zero, in which
 * case the implementation immediately returns `VK_SUCCESS`
 */
VkResult vkxCreateGraphicsPipelines(
            VkDevice device,
            uint32_t createInfoCount,
            const VkxGraphicsPipelineCreateInfo* pCreateInfos,
            const VkAllocationCallbacks* pAllocator,
            VkPipeline* pPipelines);

/**@}*/

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef VULKANX_PIPELINE_H
