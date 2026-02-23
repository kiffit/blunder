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
const char *title = "Blunder";

// Last frame's cloud resolution scale, to trigger refresh if it changes
double cloudScaleLast;

// Prototypes for imgui
void initImgui(State &);
void updateImgui(State &);

// =========================================== //
// Helpers
// =========================================== //
static void recreateRenderTargets(State &state, int w, int h) {
    state.screenWidth = w;
    state.screenHeight = h;

    int cloudW = int(w * state.cloudScale);
    int cloudH = int(h * state.cloudScale);

    if (state.cloudTex)
        glDeleteTextures(1, &state.cloudTex);

    if (state.sceneTex)
        glDeleteTextures(1, &state.sceneTex);

    // Cloud texture (low res)
    glGenTextures(1, &state.cloudTex);
    glBindTexture(GL_TEXTURE_2D, state.cloudTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, cloudW, cloudH);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Scene texture (full res)
    glGenTextures(1, &state.sceneTex);
    glBindTexture(GL_TEXTURE_2D, state.sceneTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, w, h);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // FBO
    if (!state.sceneFBO)
        glGenFramebuffers(1, &state.sceneFBO);

    glBindFramebuffer(GL_FRAMEBUFFER, state.sceneFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, state.sceneTex, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Scene FBO incomplete\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    State *state = static_cast<State *>(glfwGetWindowUserPointer(window));
    glViewport(0, 0, width, height);
    recreateRenderTargets(*state, width, height);
}

// =========================================== //
// init, update
// =========================================== //
void RenderComponent::init(State &state) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Instantiate window
    state.window = glfwCreateWindow(state.screenWidth, state.screenHeight, title, nullptr, nullptr);
    if (state.window == nullptr) {
        std::cerr << "Failed to initialize GLFW window!" << std::endl;
    }

    // Set window to be current
    glfwMakeContextCurrent(state.window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
    }

    // Register callback for window resizing
    glfwSetWindowUserPointer(state.window, &state);
    glfwSetFramebufferSizeCallback(state.window, framebuffer_size_callback);

    // Set dimension limits
    glfwSetWindowSizeLimits(state.window, minWidth, minHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);

    // Fix HiDPI issue
    int fbW = 0;
    int fbH = 0;
    glfwGetFramebufferSize(state.window, &fbW, &fbH);
    state.screenWidth = fbW;
    state.screenHeight = fbH;
    glViewport(0, 0, fbW, fbH);
    cloudScaleLast = state.cloudScale;
    recreateRenderTargets(state, fbW, fbH);

    // Optional: disable vsync
    // glfwSwapInterval(0);

    // Initialize imgui
    initImgui(state);

    // Fullscreen VAO
    glGenVertexArrays(1, &state.fullscreenVAO);

    // Shaders
    state.cloudShader.loadCompute("../src/shaders/cloud.comp");
    state.atmosphereShader.loadGraphics("../src/shaders/fullscreen.vert", "../src/shaders/atmosphere.frag");
    state.postShader.loadGraphics("../src/shaders/fullscreen.vert", "../src/shaders/post.frag");

    // Render targets
    recreateRenderTargets(state, state.screenWidth, state.screenHeight);
}

void RenderComponent::update(State &state) {
    // Reload textures if cloud scale changes
    if (state.cloudScale != cloudScaleLast) {
        recreateRenderTargets(state, state.screenWidth, state.screenHeight);
    }

    int cloudW = int(state.screenWidth * state.cloudScale);
    int cloudH = int(state.screenHeight * state.cloudScale);

    // Clouds
    state.cloudShader.use();
    state.cloudShader.setFloat("uTime", state.utime);

    glBindImageTexture(0, state.cloudTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
    glDispatchCompute((cloudW + 7) / 8, (cloudH + 7) / 8, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // Atmosphere / Merge
    glBindFramebuffer(GL_FRAMEBUFFER, state.sceneFBO);
    glViewport(0, 0, state.screenWidth, state.screenHeight);
    glClear(GL_COLOR_BUFFER_BIT);

    state.atmosphereShader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state.cloudTex);
    state.atmosphereShader.setInt("uCloudTex", 0);

    glBindVertexArray(state.fullscreenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // PostFX
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, state.screenWidth, state.screenHeight);
    glClear(GL_COLOR_BUFFER_BIT);

    state.postShader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state.sceneTex);
    state.postShader.setInt("uSceneTex", 0);

    glBindVertexArray(state.fullscreenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // imgui
    updateImgui(state);
    glfwSwapBuffers(state.window);
}

// =========================================== //
// imgui
// =========================================== //

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
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(sidebar_width, 0), ImGuiCond_Always);

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

        ImGui::SliderFloat("Resolution Scale", &state.cloudScale, 0.01f, 1.0f);

        ImGui::EndChild();
        ImGui::EndGroup();
        ImGui::Spacing();
    }

    // Performance Tools
    if (ImGui::CollapsingHeader("Scene Metrics", ImGuiTreeNodeFlags_DefaultOpen)) {

        ImGui::Spacing();
        ImGui::BeginGroup();
        ImGui::BeginChild("PerformanceBox", ImVec2(ImGui::GetContentRegionAvail().x, 0),
                          ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders);

        ImGui::SeparatorText("Performance");
        ImGui::Text("FPS: %d", (int)(1 / state.dtime));
        ImGui::Text("Frametime: %.2f ms", state.dtime * 1000);
        ImGui::Text("Viewport: (%d, %d)", state.screenWidth, state.screenHeight);
        ImGui::Text("Cloud Size: (%d, %d)", (int)(state.screenWidth * state.cloudScale), (int)(state.screenHeight * state.cloudScale));

        ImGui::SeparatorText("Camera");
        ImGui::Text("Position: (%.2f, %.2f, %.2f)", state.cameraPosition.x, state.cameraPosition.y, state.cameraPosition.z);
        ImGui::Text("Pitch: %.2f", state.pitch);
        ImGui::Text("Yaw: %.2f", state.yaw);

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
