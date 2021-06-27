cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(MandelbrotProject VERSION 0.2.1)
set(CMAKE_CXX_STANDARD 17)

# set output directories for release and debug
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../)

# include directories
include_directories(${CMAKE_SOURCE_DIR}/lib)

# static analysis (only debug)
include(cmake/StaticAnalysis.cmake)

# executable
add_executable(MandelbrotApp src/main.cpp)
target_sources(MandelbrotApp PRIVATE
        src/main.cpp

        #src/shader.cpp
        src/utils.cpp
        src/saved_view.cpp
        #src/imgui.cpp
)

# set compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(MandelbrotApp)

# sanitizer options if supported by compiler (only debug)
include(cmake/Sanitizers.cmake)
enable_sanitizers(MandelbrotApp)

# build shaders
set(SHADERS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/res/shaders)

set(SHADERS_BUILD_GOAL
        ${SHADERS_SOURCE_DIR}/quad_vert.spv
        ${SHADERS_SOURCE_DIR}/mandelbrot_frag.spv
)

add_custom_command(
        COMMAND ${SHADERS_SOURCE_DIR}/compile.sh
        WORKING_DIRECTORY ${SHADERS_SOURCE_DIR}
        OUTPUT ${SHADERS_BUILD_GOAL}
)

add_custom_target(
        shaders ALL
        DEPENDS ${SHADERS_BUILD_GOAL}
)

add_dependencies(MandelbrotApp shaders)

# ImGui Library
set(IMGUI_PATH  ${CMAKE_SOURCE_DIR}/lib/imgui)

file(GLOB IMGUI_SOURCES
        ${IMGUI_PATH}/*.cpp)

file(GLOB IMGUI_MISC_SOURCES
        ${IMGUI_PATH}/misc/*.cpp)

set(IMGUI_SOURCES
        ${IMGUI_SOURCES}
        ${IMGUI_MISC_SOURCES}
        ${IMGUI_PATH}/backends/imgui_impl_vulkan.cpp
        ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp)

add_library(ImGui STATIC ${IMGUI_SOURCES})
target_include_directories(ImGui PUBLIC ${IMGUI_PATH})
# sanitizer options if supported by compiler (only debug)
#enable_sanitizers(ImGui)

# link ImGui
target_link_libraries(MandelbrotApp PUBLIC ImGui)

# find vulkan
SET(VULKAN_SDK /home/nic/vulkan/vulkan-sdk/1.2.154.0/x86_64)
SET(Vulkan_LIBRARY ${VULKAN_SDK}/lib/libvulkan.so)
SET(Vulkan_INCLUDE_DIR ${VULKAN_SDK}/include)

# link vulkan
find_package(Vulkan REQUIRED FATAL_ERROR)
target_link_libraries(MandelbrotApp PUBLIC Vulkan::Vulkan)

# find glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_SOURCE_DIR}/lib/glfw)

# link glfw
target_link_libraries(MandelbrotApp PUBLIC glfw)

# CPack
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
