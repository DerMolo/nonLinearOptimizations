# Top-level CMake project file
cmake_minimum_required(VERSION 3.8)

# Project name and version
project(CmakeTest VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent module
include(FetchContent)

# Fetch GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8 # Adjust to the desired version
)
FetchContent_MakeAvailable(glfw)

# Fetch ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.89.5 # Adjust to the desired version
)
FetchContent_MakeAvailable(imgui)

# Fetch ImPlot
FetchContent_Declare(
    implot
    GIT_REPOSITORY https://github.com/epezent/implot.git
    GIT_TAG v0.14 # Adjust to the desired version
)
FetchContent_MakeAvailable(implot)

# Fetch ExprTk
FetchContent_Declare(
    exprtk
    GIT_REPOSITORY https://github.com/ArashPartow/exprtk.git
)
FetchContent_MakeAvailable(exprtk)

# Include directories for all dependencies
include_directories(
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${implot_SOURCE_DIR}
    ${exprtk_SOURCE_DIR}
    ${glfw_SOURCE_DIR}/include
)

# Add source files
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CmakeTest)
set(SOURCES
    ${SRC_DIR}/CmakeTest.cpp
)

# ImGui source files
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
)

# ImPlot source files
set(IMPLOT_SOURCES
    ${implot_SOURCE_DIR}/implot.cpp
    ${implot_SOURCE_DIR}/implot_items.cpp
)

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES} ${IMPLOT_SOURCES})

# Find OpenGL
find_package(OpenGL REQUIRED)
if (NOT OpenGL_FOUND)
    message(FATAL_ERROR "OpenGL not found! Ensure it is installed and available.")
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME}
    glfw
    OpenGL::GL
)

# Add compile options for MSVC to handle large object files
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /bigobj)
endif()
