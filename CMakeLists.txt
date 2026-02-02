cmake_minimum_required(VERSION 3.10)
project(blunder)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find GLFW (installed via system package manager)
find_package(glfw3 3.3 REQUIRED)

# Add executable
add_executable(app
    src/main.cpp
    src/glad.c
)

# Include directories
target_include_directories(app PRIVATE
    include
)

# Link libraries
target_link_libraries(app
    glfw
    OpenGL::GL
)
