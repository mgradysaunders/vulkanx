# CMake version.
cmake_minimum_required(VERSION 3.14)

# Project.
project(vulkanx VERSION 1.0.0 LANGUAGES C)

# Set release.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set compile flags.
set(CMAKE_CFLAGS_DEBUG "-Wall -Wextra -g")
set(CMAKE_CFLAGS_RELEASE "-Wall -Wextra -O3 -DNDEBUG ")

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

find_package(Vulkan REQUIRED)

add_library(vulkanx STATIC ${SOURCES})

# Set C11.
set_target_properties(
    vulkanx
    PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED True
    )
target_include_directories(
    vulkanx
    PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    "${Vulkan_INCLUDE_DIRS}"
    )

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}_Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
    )
set(INCLUDE_DIR include)
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    PATH_VARS INCLUDE_DIR
    INSTALL_DESTINATION
    ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
    )

install(
    EXPORT ${PROJECT_NAME}_Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
    )
install(
    FILES
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION
    "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake"
    )
install(
    DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
    DESTINATION include
    )
