#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>   // Vcpkg provides this via the 'glad' package
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

// --- THE WORD WIZARD LOGIC ---
class WordWizard {
private:
    std::unordered_map<std::string, std::vector<std::string>> dictionary;
    char inputBuffer[128] = "";
    std::vector<std::string> results;

    std::string sortString(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        std::sort(s.begin(), s.end());
        // Remove non-alpha chars for cleaner matching
        s.erase(std::remove_if(s.begin(), s.end(), [](char c) { return !isalpha(c); }), s.end());
        return s;
    }

public:
    void Init() {
        // Hardcoded dictionary for demo purposes
        std::vector<std::string> words = {
            "star", "rats", "arts", "tsar",
            "apple", "pale", "leap", "plea",
            "listen", "silent", "enlist",
            "evil", "vile", "live", "veil"
        };

        for (const auto& w : words) {
            dictionary[sortString(w)].push_back(w);
        }
    }

    void RenderUI() {
        ImGui::Begin("Word Wizard: Anagram Finder");

        ImGui::Text("Type letters (e.g., 'astr'):");

        // This input box triggers the search on every keystroke
        if (ImGui::InputText("##letters", inputBuffer, IM_ARRAYSIZE(inputBuffer))) {
            std::string key = sortString(inputBuffer);
            if (dictionary.count(key)) {
                results = dictionary[key];
            }
            else {
                results.clear();
            }
        }

        ImGui::Separator();

        if (strlen(inputBuffer) == 0) {
            ImGui::TextDisabled("Waiting for input...");
        }
        else if (results.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "No anagrams found.");
        }
        else {
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Found %d match(es):", (int)results.size());
            for (const auto& word : results) {
                ImGui::BulletText("%s", word.c_str());
            }
        }

        ImGui::End();
    }
};

// --- BOILERPLATE: GLFW & MAIN LOOP ---
static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main() {
    // 1. Setup Window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui Word Wizard", NULL, NULL);
    if (window == NULL) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // 2. Setup GLAD (Load OpenGL functions)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return 1;
    }

    // 3. Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // 4. Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 5. Initialize our App Logic
    WordWizard app;
    app.Init();

    // 6. Main Loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render our app
        app.RenderUI();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // 7. Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}