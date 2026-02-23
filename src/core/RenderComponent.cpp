#include "RenderComponent.hpp"

// std::max
#include <algorithm>

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
void framebuffer_size_callback(GLFWwindow *window, int width, int height) { glViewport(0, 0, width, height); }

void RenderComponent::init(State &state) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Instantiate window
    state.window = glfwCreateWindow(startingWidth, startingHeight, title, nullptr, nullptr);
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
    glfwSetWindowSizeLimits(state.window, minWidth, minHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);

    // Optional: disable vsync
    // glfwSwapInterval(0);

    // Initialize imgui
    initImgui(state);
}

void RenderComponent::update(State &state) {
    // Rendering commands here
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render imgui
    updateImgui(state);

    // Swap buffers
    glfwSwapBuffers(state.window);
}

// imgui variables
constexpr int font_size = 14;
constexpr int sidebar_width = 350;

// Garbage fillers
bool fakebool = false;
int fakeint = 0;
float fakefloat = 0;

void initImgui(State &state) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // Load font
    io.Fonts->AddFontFromFileTTF("../include/imgui/misc/fonts/Roboto-Medium.ttf", font_size);

    // Dark mode
    ImGui::StyleColorsDark();

    // Initialize backends
    ImGui_ImplGlfw_InitForOpenGL(state.window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void updateImgui(State &state) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuiIO &io = ImGui::GetIO();

    // Sidebar
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(sidebar_width, io.DisplaySize.y), ImGuiCond_Always);

    //  Flags, for keeping it a panel
    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;

    // Panel begin
    ImGui::Begin("Blunder Options", nullptr, flags);

    // Cloud Settings
    if (ImGui::CollapsingHeader("Cloud Settings", ImGuiTreeNodeFlags_DefaultOpen)) {

        ImGui::Spacing();
        ImGui::BeginGroup();
        ImGui::BeginChild("CloudBox", ImVec2(ImGui::GetContentRegionAvail().x, 0),
                          ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders);

        ImGui::Checkbox("Enable Shadows", &fakebool);
        ImGui::SliderFloat("Exposure", &fakefloat, 0.0f, 5.0f);
        ImGui::SliderInt("Samples", &fakeint, 1, 256);

        ImGui::EndChild();
        ImGui::EndGroup();
        ImGui::Spacing();
    }

    // Performance Tools
    if (ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen)) {

        ImGui::Spacing();
        ImGui::BeginGroup();
        ImGui::BeginChild("PerformanceBox", ImVec2(ImGui::GetContentRegionAvail().x, 0),
                          ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders);

        ImGui::Text("FPS: %d", (int)(1 / state.dtime));
        ImGui::Text("Frametime: %.2f ms", state.dtime * 1000);

        ImGui::EndChild();
        ImGui::EndGroup();
        ImGui::Spacing();
    }

    // Finish
    ImGui::End();

    // Render
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
