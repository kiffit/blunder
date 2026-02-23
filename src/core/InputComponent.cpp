#include "InputComponent.hpp"

// State
bool canReload = true;

void InputComponent::init(State &state) {}

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

    // Call events
    glfwPollEvents();
}
