#include "InputComponent.hpp"

void InputComponent::init(State &state) {}

void InputComponent::update(State &state) {
    // Process input
    if (glfwGetKey(state.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(state.window, true);
    }

    // Call events
    glfwPollEvents();
}
