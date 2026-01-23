cmake_minimum_required(VERSION 3.16)
project(blunder LANGUAGES CXX)

# C++ 20 for Vulkan RAII
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output dirs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Require dependencies
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Blunder
add_executable(blunder
    src/main.cpp
)

# Link libraries
target_link_libraries(blunder
    PRIVATE
        Vulkan::Vulkan
        glfw
        glm::glm
)

# Warnings for debugging
target_compile_options(blunder PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
