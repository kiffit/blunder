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
    int screenWidth{1200};
    int screenHeight{800};
    float cloudScale{0.20f};

    // Atmosphere
    int atmosphereSamples{4};
    float sunAngle{30.f};
    float atmosphereMix = 0.6f;

    // Clouds (tunable)
    int cloudSteps = 36;
    int cloudLightSteps = 16;

    float cloudDensityScale = 0.06f;
    float cloudAbsorption = 0.2f;
    float cloudScattering = 0.05f;

    float cloudScaleFactor = 0.00002f;
    float cloudDetail = 10.0f;
    float cloudWarp = 4.0f;
    float cloudCover = 0.5f;
};