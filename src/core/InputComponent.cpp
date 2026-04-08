#include "InputComponent.hpp"
#include <glm/glm.hpp>

// State
bool canReload = true;
float yaw{90.f};
float pitch{30.f};
float speed{5.0f};
float sensitivity{0.1f};

void InputComponent::init(State &state) {
    float yawRad = glm::radians(yaw);
    float pitchRad = glm::radians(pitch);

    state.viewDir.x = cos(pitchRad) * cos(yawRad);
    state.viewDir.y = cos(pitchRad) * sin(yawRad);
    state.viewDir.z = sin(pitchRad);

    state.viewDir = glm::normalize(state.viewDir);
}

void InputComponent::update(State &state) {
    // Exit events
    if (glfwGetKey(state.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(state.window, true);
    }

    // Reload
    if (canReload && glfwGetKey(state.window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(state.window, GLFW_KEY_R) == GLFW_PRESS) {
        state.cloudShader.reload();
        state.atmosphereShader.reload();
        state.postShader.reload();
        std::cout << "Shaders reloaded!" << std::endl;
        canReload = false;
    } else if (glfwGetKey(state.window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE || glfwGetKey(state.window, GLFW_KEY_R) == GLFW_RELEASE) {
        canReload = true;
    }

    // Mouse view controls
    static double lastX = 0.0, lastY = 0.0;
    static double startX = 0.0, startY = 0.0;
    static bool dragging = false;

    if (glfwGetMouseButton(state.window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {

        double xpos, ypos;
        glfwGetCursorPos(state.window, &xpos, &ypos);

        if (!dragging) {
            dragging = true;

            startX = xpos;
            startY = ypos;

            lastX = xpos;
            lastY = ypos;

            glfwSetInputMode(state.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        float dx = float(xpos - lastX);
        float dy = float(lastY - ypos);

        lastX = xpos;
        lastY = ypos;

        yaw -= dx * sensitivity;
        pitch += dy * sensitivity;

        // Clamp pitch
        pitch = glm::clamp(pitch, -89.0f, 89.0f);

        // Wrap yaw
        if (yaw > 180.f)
            yaw -= 360.f;
        if (yaw < -180.f)
            yaw += 360.f;

        // View direction
        float yawRad = glm::radians(yaw);
        float pitchRad = glm::radians(pitch);

        state.viewDir.x = cos(pitchRad) * cos(yawRad);
        state.viewDir.y = cos(pitchRad) * sin(yawRad);
        state.viewDir.z = sin(pitchRad);

        state.viewDir = glm::normalize(state.viewDir);

    } else if (dragging) {

        dragging = false;

        glfwSetInputMode(state.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPos(state.window, startX, startY);
    }

    // Keyboard movement controls
    float yawRad = glm::radians(yaw);

    glm::vec3 forward = glm::normalize(glm::vec3(cos(yawRad), sin(yawRad), 0.0f));
    glm::vec3 right = glm::normalize(glm::vec3(-sin(yawRad), cos(yawRad), 0.0f));

    if (glfwGetKey(state.window, GLFW_KEY_W) == GLFW_PRESS)
        state.cameraPosition += forward * speed * state.dtime;

    if (glfwGetKey(state.window, GLFW_KEY_S) == GLFW_PRESS)
        state.cameraPosition -= forward * speed * state.dtime;

    if (glfwGetKey(state.window, GLFW_KEY_D) == GLFW_PRESS)
        state.cameraPosition += right * speed * state.dtime;

    if (glfwGetKey(state.window, GLFW_KEY_A) == GLFW_PRESS)
        state.cameraPosition -= right * speed * state.dtime;

    if (glfwGetKey(state.window, GLFW_KEY_SPACE) == GLFW_PRESS)
        state.cameraPosition.z += speed * state.dtime;

    if (glfwGetKey(state.window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        state.cameraPosition.z -= speed * state.dtime;

    // Call events
    glfwPollEvents();
}
