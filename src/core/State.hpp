#pragma once
#include <glm/vec3.hpp>

#include "Shader.hpp"

struct GLFWwindow;

struct State {
    // Window
    GLFWwindow *window{nullptr};

    // Movement
    glm::vec3 cameraPosition{0, 0, 0};

    // View
    glm::vec3 viewDir{0, 0, 0};
    float fov{90.f};

    // Timing
    float utime{0.f};
    float dtime{0.f};

    // Shaders
    Shader cloudShader;
    Shader atmosphereShader;
    Shader postShader;

    // Rendering
    GLuint cloudTex{0};
    GLuint sceneTex{0};
    GLuint sceneFBO{0};
    GLuint fullscreenVAO{0};

    // Resolution
    int screenWidth{800};
    int screenHeight{600};
    float cloudScale{0.5f};

    // Atmosphere
    int atmosphereSamples{4};
    float sunAngle{30.f};
    
};