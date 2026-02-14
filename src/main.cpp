#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>   // Vcpkg provides this via the 'glad' package
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>

#include "hashmap.h"

// --- THE WORD WIZARD LOGIC ---
class WordWizard {
private:
    AnagramHashMap dictionary{ "Input/words_alpha.txt" };
    char inputBuffer[128] = "";

    std::vector<std::string> resultsHash;
    std::vector<std::string> resultsLinear;

    // Naive storage of words
    std::vector<std::string> wordList;

    // Toggle for comparison
    bool showLinearComparison = false;

    // --- Performance statistics ---
    size_t lookupCount = 0;

    double hashTotalTimeMs = 0.0;
    double hashLastTimeMs = 0.0;

    double linearTotalTimeMs = 0.0;
    double linearLastTimeMs = 0.0;

    std::string sortString(std::string s) {
        s.erase(std::remove_if(s.begin(), s.end(),
            [](char c) { return !isalpha(c); }), s.end());
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        std::sort(s.begin(), s.end());
        return s;
    }

    // Naive linear anagram search
    std::vector<std::string> linearSearch(const std::string& key) {
        std::vector<std::string> result;
        for (const auto& word : wordList) {
            if (sortString(word) == key) {
                result.push_back(word);
            }
        }
        return result;
    }

public:
    void Init() {
        // Load all words for naive search
        std::ifstream input("Input/words_alpha.txt");
        std::string buf;
        while (input >> buf) {
            wordList.push_back(buf);
        }
    }

    void RenderUI() {
        ImGui::Begin("Word Wizard: Anagram Finder");

        ImGui::Text("Type letters (e.g., 'astr'):");

        ImGui::Checkbox("Show performance comparison with linear search",
            &showLinearComparison);

        if (ImGui::InputText("##letters", inputBuffer, IM_ARRAYSIZE(inputBuffer))) {
            std::string key = sortString(inputBuffer);

            // --- HASH MAP TIMING ---
            auto hashStart = std::chrono::high_resolution_clock::now();

            if (dictionary.count(key)) {
                resultsHash = dictionary[key];
            }
            else {
                resultsHash.clear();
            }

            auto hashEnd = std::chrono::high_resolution_clock::now();
            hashLastTimeMs =
                std::chrono::duration<double, std::milli>(hashEnd - hashStart).count();
            hashTotalTimeMs += hashLastTimeMs;

            // --- LINEAR SEARCH (only if enabled) ---
            if (showLinearComparison) {
                auto linearStart = std::chrono::high_resolution_clock::now();
                resultsLinear = linearSearch(key);
                auto linearEnd = std::chrono::high_resolution_clock::now();

                linearLastTimeMs =
                    std::chrono::duration<double, std::milli>(linearEnd - linearStart).count();
                linearTotalTimeMs += linearLastTimeMs;
            }

            lookupCount++;
        }

        ImGui::Separator();

        if (strlen(inputBuffer) == 0) {
            ImGui::TextDisabled("Waiting for input...");
        }
        else if (resultsHash.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f),
                "No anagrams found.");
        }
        else {
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f),
                "Found %d match(es):",
                (int)resultsHash.size());
            for (const auto& word : resultsHash) {
                ImGui::BulletText("%s", word.c_str());
            }
        }

        // --- Performance UI ---
        ImGui::Separator();
        ImGui::Text("Hash Map Performance");
        ImGui::Text("Last lookup: %.4f ms", hashLastTimeMs);

        if (lookupCount > 0) {
            ImGui::Text("Average: %.4f ms",
                hashTotalTimeMs / lookupCount);
        }

        if (showLinearComparison && lookupCount > 0) {
            ImGui::Separator();
            ImGui::Text("Linear Search Performance");
            ImGui::Text("Last lookup: %.4f ms", linearLastTimeMs);
            ImGui::Text("Average: %.4f ms",
                linearTotalTimeMs / lookupCount);

            double speedup =
                (hashLastTimeMs > 0.0)
                ? (linearLastTimeMs / hashLastTimeMs)
                : 0.0;

            ImGui::TextColored(
                ImVec4(0.4f, 1.0f, 0.4f, 1.0f),
                "Hash map speedup: %.2fx",
                speedup
            );
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