#include "RenderComponent.hpp"

// imgui imports
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

// Constants
constexpr int minWidth = 400;
constexpr int minHeight = 300;
constexpr int startingWidth = 800;
constexpr int startingHeight = 600;
constexpr char *title = "Blunder";

// Prototypes for imgui
void initImgui(State &);
void updateImgui(State &);

// Window callback
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void RenderComponent::init(State &state) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Instantiate window
    state.window = glfwCreateWindow(startingWidth, startingHeight, title,
                                    nullptr, nullptr);
    if (state.window == nullptr) {
        std::cerr << "Failed to initialize GLFW window!" << std::endl;
    }

    // Set window to be current
    glfwMakeContextCurrent(state.window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
    }

    // Give OpenGL viewport dimensions
    glViewport(0, 0, startingWidth, startingHeight);

    // Register callback for window resizing
    glfwSetFramebufferSizeCallback(state.window, framebuffer_size_callback);

    // Set dimension limits
    glfwSetWindowSizeLimits(state.window, minWidth, minHeight, GLFW_DONT_CARE,
                            GLFW_DONT_CARE);

    // Initialize imgui
    initImgui(state);
}

void RenderComponent::update(State &state) {
    // Rendering commands here
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render imgui
    updateImgui(state);

    // Swap buffers
    glfwSwapBuffers(state.window);
}

// -- Extra: imgui --
bool show_demo = true;
bool enabled = false;
float speed = 0.5f;
int count = 5;
int combo_current = 0;
int list_current = 0;
int radio = 0;

const char *combo_items[] = {"Easy", "Medium", "Hard"};
const char *list_items[] = {"Apple", "Banana", "Orange", "Pear"};

void initImgui(State &state) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(state.window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void updateImgui(State &state) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // UI Begin
    ImGui::Begin("Controls");

    ImGui::Checkbox("Enable Feature", &enabled);
    ImGui::SliderFloat("Speed", &speed, 0.0f, 1.0f);
    ImGui::SliderInt("Count", &count, 0, 100);

    ImGui::Combo("Difficulty", &combo_current, combo_items,
                 IM_ARRAYSIZE(combo_items));
    ImGui::ListBox("Fruits", &list_current, list_items,
                   IM_ARRAYSIZE(list_items));

    ImGui::RadioButton("Option A", &radio, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Option B", &radio, 1);
    ImGui::End();

    // UI End
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
