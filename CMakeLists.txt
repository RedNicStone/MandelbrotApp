cmake_minimum_required(VERSION 3.2.0)
project(MandelbrotProject VERSION 0.1.0)

#include(CTest)
#enable_testing()

# Set output directories for release and debug
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin-debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin-release)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/lib/GLFW/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/GLAD/include)

# Compile definitions
add_compile_definitions(MandelbrotProject GLFW_INCLUDE_NONE)

# Exectutable
add_executable(MandelbrotApp "")
target_sources(MandelbrotApp PRIVATE
    src/main.cpp
    #lib/ImGui/main.cpp

    src/shader.h
    src/shader.cpp
    src/app_utility.h
    src/app_utility.cpp

    lib/GLAD/glad.c

    lib/ImGui/imgui.h
    lib/ImGui/imgui.cpp
    lib/ImGui/imgui_internal.h
    lib/ImGui/imgui_tables.cpp
    lib/ImGui/imgui_widgets.cpp
    lib/ImGui/imgui_draw.cpp
    lib/ImGui/imstb_rectpack.h
    lib/ImGui/imstb_textedit.h
    lib/ImGui/imstb_truetype.h
    lib/ImGui/imconfig.h
    lib/ImGui/imgui_impl_opengl3.h
    lib/ImGui/imgui_impl_opengl3.cpp
    lib/ImGui/imgui_impl_glfw.cpp
    lib/ImGui/imgui_impl_glfw.h

)

set_property(TARGET MandelbrotApp PROPERTY CXX_STANDARD 20)

# link glfw
#find_package(glfw3 3.3 REQUIRED) # doesnt work (with X11)
#target_link_libraries(MandelbrotApp pthread glfw) # doesnt work (with X11)
target_link_libraries(MandelbrotApp pthread ${CMAKE_SOURCE_DIR}/lib/GLFW/bin/X11-release/libglfw3.a)

# needed for glad (for opening libraries)
target_link_libraries(MandelbrotApp ${CMAKE_DL_LIBS})

# CPack
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
