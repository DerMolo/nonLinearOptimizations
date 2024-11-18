# Top-level CMake project file
cmake_minimum_required(VERSION 3.8)

# Project name and version
project(CmakeTest VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define source directory
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CmakeTest)

# Specify the source files explicitly
set(SOURCES
    ${SRC_DIR}/CmakeTest.cpp
)

# Add library directories
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui)
set(IMPLOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/implot)
set(EXPRTK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/exprtk)
set(GLFW_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/build/src) # Adjusted directory

# Ensure GLFW build paths are valid
if (NOT EXISTS ${GLFW_BUILD_DIR}/Release)
    message(FATAL_ERROR "GLFW directory not found or not built at ${GLFW_BUILD_DIR}/Release")
endif()

# Include directories
set(GLFW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/include)
include_directories(
    ${GLFW_INCLUDE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${IMPLOT_DIR}
    ${EXPRTK_DIR}
)

# Add ImGui and ImPlot sources
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
)

set(IMPLOT_SOURCES
    ${IMPLOT_DIR}/implot.cpp
    ${IMPLOT_DIR}/implot_items.cpp
)

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES} ${IMPLOT_SOURCES})

# Find GLFW library
find_library(GLFW_LIB glfw3 PATHS ${GLFW_BUILD_DIR}/Release NO_DEFAULT_PATH)

if (NOT GLFW_LIB)
    message(FATAL_ERROR "GLFW library (glfw3.lib) not found in ${GLFW_BUILD_DIR}/Release")
endif()

# Find OpenGL
find_package(OpenGL REQUIRED)
if (NOT OpenGL_FOUND)
    message(FATAL_ERROR "OpenGL not found! Ensure it is installed and available.")
endif()

# Link libraries AFTER target creation
target_link_libraries(${PROJECT_NAME} ${GLFW_LIB} OpenGL::GL)

# Add compile options for MSVC to handle large object files
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /bigobj)
endif()
