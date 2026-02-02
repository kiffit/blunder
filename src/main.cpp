// System includes
#include <iostream>

// GLFW/GLAD includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Prototypes
void framebuffer_size_callback(GLFWwindow *, int, int);
void processInput(GLFWwindow *);

int main()
{
    // Initialization
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Window object
    GLFWwindow *window = glfwCreateWindow(800, 600, "Blunder", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to initialize GLFW window!" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);

    // GLAD Initialization
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return -1;
    }

    // Give OpenGL viewport dimensions
    glViewport(0, 0, 800, 600);

    // Register callback for window resizing
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Primary loop
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // Rendering commands here...
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Call events, swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // Clean up
    glfwTerminate();
    return 0;
}

// Implementations
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}