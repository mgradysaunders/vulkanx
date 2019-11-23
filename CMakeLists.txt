# CMake version.
cmake_minimum_required(VERSION 3.10)

# Project.
project(vulkanx VERSION 1.0.0 LANGUAGES C)

# Set release.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set compile flags.
set(CMAKE_CXX_FLAGS "-Wall -Wextra -march=native -mtune=native")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Glob sources.
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.c")

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
