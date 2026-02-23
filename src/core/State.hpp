#pragma once
#include <glm/vec3.hpp>

struct GLFWwindow;

struct State {
    // Window
    GLFWwindow *window{nullptr};

    // Movement
    glm::vec3 cameraPosition{0,0,0};

    // View
    float yaw{-90.f};
    float pitch{0.f};

    // Timing
    float utime{0.f};
    float dtime{0.f};
};