# CMake version.
cmake_minimum_required(VERSION 3.10)

# Project.
project(vulkanx VERSION 1.0.0 LANGUAGES C)

# Set release.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set compile flags.
set(CMAKE_CFLAGS_DEBUG "-g")
set(CMAKE_CFLAGS_RELEASE "-O3 -DNDEBUG ")

# Glob sources.
# file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.c")
set(SOURCES
    src/buffer.c
    src/command_buffer.c
    src/descriptor_set.c
    src/image.c
    src/memory.c
    src/pipeline.c
    src/result.c
    src/setup.c
    src/shader.c
    src/swapchain.c
    src/vulkanx_SDL.c)

# Add main library.
add_library(vulkanx STATIC ${SOURCES})

# Set C++17.
set_target_properties(
    vulkanx
    PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED True
    )

# Include directories.
target_include_directories(
    vulkanx
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )
