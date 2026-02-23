#include "Application.hpp"

void Application::run() {
    init();

    while (!glfwWindowShouldClose(state->window)) {

        // Adjust timings
        float currentTime = glfwGetTime();
        state->dtime = currentTime - state->utime;
        state->utime = currentTime;

        // Update
        input->update(*state);
        render->update(*state);
    }

    cleanUp();
}

void Application::init() {
    state = new State();
    input = new InputComponent();
    render = new RenderComponent();

    input->init(*state);
    render->init(*state);
}

void Application::cleanUp() {
    glfwTerminate();

    delete state;
    delete input;
    delete render;

    state = nullptr;
    input = nullptr;
    render = nullptr;
}
