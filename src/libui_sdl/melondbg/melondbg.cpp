#include <thread>
#include "melondbg.h"
#include "../imgui/imgui.h"
#include "../imgui/examples/imgui_impl_glfw.h"
#include "../imgui/examples/imgui_impl_opengl3.h"
#include "../glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "../../NDS.h"
#include "registers.h"
#include "memoryeditor.h"
#include "disassembler.h"

namespace melonDBG {
std::thread* dbg_thread;
bool should_thread_stop = false;

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void Run() {
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return;

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "melonDBG", NULL, NULL);
    if (window == NULL)
        return;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    bool err = gladLoadGL() == 0;
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Initialize windows
    std::vector<Window*> windows;
    windows.push_back(new RegisterWindow("ARM9", NDS::ARM9));
    windows.push_back(new RegisterWindow("ARM7", NDS::ARM7));

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();

        if (should_thread_stop)
            glfwSetWindowShouldClose(window, 1);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Draw menu bar
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("View")) {
                if (ImGui::BeginMenu("Add new...")) {
                    if (ImGui::MenuItem("Memory Editor"))
                        windows.push_back(new MemoryEditorWindow());
                    if (ImGui::MenuItem("Disassembler"))
                        windows.push_back(new DisassemblerWindow());
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Windows")) {
                    for (const auto& window : windows)
                        ImGui::Checkbox(window->GetTitle().c_str(), &window->open);
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Draw windows
        for (const auto& window : windows) {
            if (window->open)
                window->Draw();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void StartThread() {
    dbg_thread = new std::thread(Run);
}

void StopThread() {
    should_thread_stop = true;
    dbg_thread->join();
}
}