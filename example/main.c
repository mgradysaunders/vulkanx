#include <stdio.h>
#include <math.h>
#include <vulkanx_SDL.h>
#include "shader_vert.inl"
#include "shader_frag.inl"

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "failed to initialize SDL\n");
        exit(EXIT_FAILURE);
    }

    const char* debugLayer = "VK_LAYER_KHRONOS_validation";
    VkxInstanceCreateInfo instanceCreateInfo = {
        .pApplicationName = "Example",
        .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
        .pEngineName = "Example",
        .engineVersion = VK_MAKE_VERSION(0, 0, 1),
        .apiVersion = VK_API_VERSION_1_2,
        .requestedLayerCount = 0,
        .ppRequestedLayerNames = NULL,
        .requestedExtensionCount = 0,
        .ppRequestedExtensionNames = NULL
    };

    VkxSDLWindow window;
    VkxSDLWindowCreateInfo windowCreateInfo = {
        .positionX = SDL_WINDOWPOS_UNDEFINED,
        .positionY = SDL_WINDOWPOS_UNDEFINED,
        .sizeX = 1280,
        .sizeY = 720,
        .windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
        .pInstanceCreateInfo = &instanceCreateInfo,
        .overrideCommandPoolCount = 0,
        .pOverrideCommandPoolCreateFlags = NULL
    };
    vkxCreateSDLWindowOrExit(&windowCreateInfo, &window);

    VkRenderPass renderPass;
    {
        VkAttachmentDescription colorAtt = {
            .flags = 0,
            .format = window.swapchain.surfaceFormat.format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        };
        VkAttachmentReference colorAttRef = {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };
        VkSubpassDescription subpassDescr = {
            .flags = 0,
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .inputAttachmentCount = 0,
            .pInputAttachments = NULL,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttRef,
            .pResolveAttachments = NULL,
            .pDepthStencilAttachment = NULL,
            .preserveAttachmentCount = 0,
            .pPreserveAttachments = NULL
        };
        VkSubpassDependency subpassDep = {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = 0,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
        };
        VkRenderPassCreateInfo renderPassCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .attachmentCount = 1,
            .pAttachments = &colorAtt,
            .subpassCount = 1,
            .pSubpasses = &subpassDescr,
            .dependencyCount = 1,
            .pDependencies = &subpassDep
        };
        VkResult result = vkxSwapchainSetupRenderPass(
                &window.swapchain, &renderPassCreateInfo, NULL);
        if (result != VK_SUCCESS) {
            return EXIT_FAILURE;
        }
    }

    uint32_t imageCount = window.swapchain.imageCount;

    VkxShaderModuleGroup modules;
    {
        VkxShaderModuleCreateInfo moduleCreateInfos[2] = {
            {.stage = VK_SHADER_STAGE_VERTEX_BIT,
             .codeSize = sizeof(shaderVert),
             .pCode = &shaderVert[0], .pCodeFilename = NULL},
            {.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
             .codeSize = sizeof(shaderFrag),
             .pCode = &shaderFrag[0], .pCodeFilename = NULL}
        };
        VkResult result = vkxCreateShaderModuleGroup(
                window.device.device, 
                2, &moduleCreateInfos[0], NULL, &modules);
        if (result != VK_SUCCESS) {
            return EXIT_FAILURE;
        }
    }

    VkPipelineLayout layout;
    {
        VkPipelineLayoutCreateInfo layoutCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .setLayoutCount = 0,
            .pSetLayouts = NULL,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = NULL
        };
        VkResult result = vkCreatePipelineLayout(
                window.device.device, 
                &layoutCreateInfo, NULL, 
                &layout);
        if (result != VK_SUCCESS) {
            return EXIT_FAILURE;
        }
    }

    VkPipeline pipeline;
    {
        VkVertexInputBindingDescription bindings[1] = {
            {.binding = 0,
             .stride = sizeof(float) * 5,
             .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}
        };
        VkVertexInputAttributeDescription attributes[2] = {
            {.location = 0, .binding = 0, 
             .format = VK_FORMAT_R32G32B32_SFLOAT,
             .offset = 0},
            {.location = 1, .binding = 0,
             .format = VK_FORMAT_R32G32_SFLOAT,
             .offset = sizeof(float) * 3}
        };
        VkxGraphicsPipelineInputState inputState = {
            .bindingCount = 1, .pBindings = &bindings[0],
            .attributeCount = 2, .pAttributes = &attributes[0],
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .primitiveRestartEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .patchControlPoints = 0,
            .lineWidth = 0.0f
        };
        VkViewport viewport = {
            .x = 0,
            .y = 0,
            .width = window.swapchain.imageExtent.width,
            .height = window.swapchain.imageExtent.height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };
        VkDynamicState dynamicStates[2] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineColorBlendAttachmentState blendAttachment = {
            .blendEnable = VK_FALSE,
            .srcColorBlendFactor = 0,
            .dstColorBlendFactor = 0,
            .colorBlendOp = 0,
            .srcAlphaBlendFactor = 0,
            .dstAlphaBlendFactor = 0,
            .alphaBlendOp = 0,
            .colorWriteMask = 0
        };
        VkxGraphicsPipelineCreateInfo pipelineCreateInfo = {
            .stageCount = 2,
            .pStages = modules.pStageCreateInfos,
            .pInputState = &inputState,
            .pDepthState = NULL,
            .pStencilState = NULL,
            .pMultisampleState = NULL,
            .viewportCount = 1, 
            .pViewports = &viewport,
            .pViewportScissors = NULL,
            .logicOpEnable = VK_FALSE, 
            .logicOp = 0,
            .blendAttachmentCount = 1, 
            .pBlendAttachments = &blendAttachment,
            .blendConstants = {0.f, 0.f, 0.f, 0.f},
            .dynamicStateCount = 2, 
            .pDynamicStates = &dynamicStates[0],
            .layout = layout,
            .renderPass = window.swapchain.renderPass, 
            .subpass = 0,
            .basePipeline = -1
        };
        VkResult result = 
        vkxCreateGraphicsPipelines(
                window.device.device, 
                1, &pipelineCreateInfo, NULL, &pipeline);
        if (result != VK_SUCCESS) {
            return EXIT_FAILURE;
        }
    }

    int frame = 0;
    VkBool32 quit = VK_FALSE;
    while (quit == VK_FALSE) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = VK_TRUE;
            }
        }
        VkBool32 resize = VK_FALSE;
        VkResult result = vkxSwapchainAcquireNextImage(
                &window.swapchain, UINT64_MAX);
        if (result == VK_SUCCESS) {
            frame++;
            float theta = frame / 1000.0f;
            VkCommandBuffer commandBuffer = 
                window.swapchain.activeCommandBuffer;
            vkxBeginCommandBuffer(commandBuffer, 0, NULL);
            VkClearValue clearColor = {
                .color = {fabsf(cosf(theta)), fabsf(sinf(theta)), 0.0f, 1.0f}
            };
            VkRenderPassBeginInfo renderPassInfo = {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .renderPass = window.swapchain.renderPass,
                .framebuffer = window.swapchain.activeFramebuffer,
                .renderArea = {
                    .offset = {0, 0},
                    .extent = window.swapchain.imageExtent
                },
                .clearValueCount = 1,
                .pClearValues = &clearColor
            };
            vkCmdBeginRenderPass(
                    commandBuffer, &renderPassInfo,
                    VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(
                    commandBuffer, 
                    VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
            vkCmdEndRenderPass(commandBuffer);
            if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
                exit(EXIT_FAILURE);

            result = vkxSwapchainSubmit(&window.swapchain);
            if (result != VK_SUCCESS) {
                fprintf(stderr, "vkxSwapchainSubmit failure\n");
                exit(EXIT_FAILURE);
            }
            else {
                result = vkxSwapchainPresent(&window.swapchain, 0, NULL);
                if (result != VK_SUCCESS)
                    resize = VK_TRUE;
            }
        }
        else {
            resize = VK_TRUE;
        }

        if (resize == VK_TRUE) {
            vkxSDLWindowResizeSwapchainOrExit(&window);
        }
    }

    vkDeviceWaitIdle(window.device.device);
    vkDestroyPipeline(window.device.device, pipeline, NULL);
    vkDestroyPipelineLayout(window.device.device, layout, NULL);
    vkxDestroyShaderModuleGroup(window.device.device, &modules, NULL);
    vkxDestroySDLWindow(&window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
