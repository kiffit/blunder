cmake_minimum_required(VERSION 3.10)
project(blunder)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_policy(SET CMP0072 NEW)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)

file(GLOB_RECURSE SRC_FILES
    src/*.cpp
    src/*.c
)

# ---- ImGui (compiled into your app) ----
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/include/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(app
    ${SRC_FILES}
    ${IMGUI_SOURCES}
)

target_include_directories(app PRIVATE
    src
    include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# If this fails, swap `glfw` for `glfw3` or `glfw3::glfw`
target_link_libraries(app PRIVATE
    glfw
    OpenGL::GL
    glm::glm
)