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
#include <vulkanx/result.h>

const char* vkxResultName(VkResult result)
{
    const char* str = "VK_(unknown)";
    switch (result) {
    #define RESULT_CASE(res) case res: str = #res; break;
            RESULT_CASE(VK_SUCCESS);
            RESULT_CASE(VK_NOT_READY);
            RESULT_CASE(VK_TIMEOUT);
            RESULT_CASE(VK_EVENT_SET);
            RESULT_CASE(VK_EVENT_RESET);
            RESULT_CASE(VK_INCOMPLETE);
            RESULT_CASE(VK_ERROR_OUT_OF_HOST_MEMORY);
            RESULT_CASE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
            RESULT_CASE(VK_ERROR_INITIALIZATION_FAILED);
            RESULT_CASE(VK_ERROR_DEVICE_LOST);
            RESULT_CASE(VK_ERROR_MEMORY_MAP_FAILED);
            RESULT_CASE(VK_ERROR_LAYER_NOT_PRESENT);
            RESULT_CASE(VK_ERROR_EXTENSION_NOT_PRESENT);
            RESULT_CASE(VK_ERROR_FEATURE_NOT_PRESENT);
            RESULT_CASE(VK_ERROR_INCOMPATIBLE_DRIVER);
            RESULT_CASE(VK_ERROR_TOO_MANY_OBJECTS);
            RESULT_CASE(VK_ERROR_FORMAT_NOT_SUPPORTED);
            RESULT_CASE(VK_ERROR_FRAGMENTED_POOL);
            RESULT_CASE(VK_ERROR_OUT_OF_POOL_MEMORY);
            RESULT_CASE(VK_ERROR_INVALID_EXTERNAL_HANDLE);
    #undef RESULT_CASE
        default: break;
    }
    return str;
}
