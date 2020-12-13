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
#ifndef VULKANX_SDL_H
#define VULKANX_SDL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkanx.h>

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/**
 * @defgroup SDL SDL
 *
 * `<vulkanx_SDL.h>`
 */
/**@{*/

/**
 * @brief SDL window.
 */
typedef struct VkxSDLWindow_
{
    /** @brief The window handle. */
    SDL_Window* window;

    /** @brief Enabled instance layer count. */
    uint32_t enabledLayerCount;

    /** @brief Enabled instance layer names. */
    char** ppEnabledLayerNames;

    /** @brief Enabled instance extension count. */
    uint32_t enabledExtensionCount;

    /** @brief Enabled instance extension names. */
    char** ppEnabledExtensionNames;

    /** @brief Instance. */
    VkInstance instance;

    /** @brief Device. */
    VkxDevice device;

    /** @brief Swapchain surface. */
    VkSurfaceKHR swapchainSurface;

    /** @brief Swapchain. */
    VkxSwapchain swapchain;
}
VkxSDLWindow;

/**
 * @brief SDL window create info.
 */
typedef struct VkxSDLWindowCreateInfo_
{
    /** @brief Position X. */
    int positionX;

    /** @brief Position Y. */
    int positionY;

    /** @brief Size X. */
    int sizeX;

    /** @brief Size Y. */
    int sizeY;

    /** @brief SDL window flags. */
    uint32_t windowFlags;

    /** @brief Instance create info. */
    const VkxInstanceCreateInfo* pInstanceCreateInfo;

    /** @brief _Optional_. Override command pool count. */
    uint32_t overrideCommandPoolCount;

    /** @brief _Optional_. Override command pool create flags. */
    const VkCommandPoolCreateFlags* pOverrideCommandPoolCreateFlags;
}
VkxSDLWindowCreateInfo;

/**
 * @brief Create SDL window, or report error and exit.
 */
void vkxCreateSDLWindowOrExit(
        const VkxSDLWindowCreateInfo* pCreateInfo, VkxSDLWindow* pWindow);

/**
 * @brief Destroy SDL window.
 */
void vkxDestroySDLWindow(VkxSDLWindow* pWindow);

/**
 * @brief Resize swapchain, or report error and exit.
 */
void vkxSDLWindowResizeSwapchainOrExit(VkxSDLWindow* pWindow);

/**@}*/

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef VULKANX_SDL_H
