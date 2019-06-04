#include <thread>
#include "melondbg.h"
#include "../imgui/imgui.h"
#include "../imgui/examples/imgui_impl_glfw.h"
#include "../imgui/examples/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "NDS.h"

namespace melonDBG {
std::thread* dbg_thread;
bool should_thread_stop = false;

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
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
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
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
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    bool show_demo_window = false;
    bool show_arm9_registers = true;
    bool show_arm7_registers = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        if (should_thread_stop)
            glfwSetWindowShouldClose(window, 1);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("ARM9 Registers", &show_arm9_registers);
            ImGui::Checkbox("ARM7 Registers", &show_arm7_registers);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        if (show_arm9_registers)
        {
            ImGui::Begin("ARM9", &show_arm9_registers);
            ImGui::Text("R0  %08x", NDS::ARM9->R[0]);
            ImGui::Text("R1  %08x", NDS::ARM9->R[1]);
            ImGui::Text("R2  %08x", NDS::ARM9->R[2]);
            ImGui::Text("R3  %08x", NDS::ARM9->R[3]);
            ImGui::Text("R4  %08x", NDS::ARM9->R[4]);
            ImGui::Text("R5  %08x", NDS::ARM9->R[5]);
            ImGui::Text("R6  %08x", NDS::ARM9->R[6]);
            ImGui::Text("R7  %08x", NDS::ARM9->R[7]);
            ImGui::Text("R8  %08x", NDS::ARM9->R[8]);
            ImGui::Text("R9  %08x", NDS::ARM9->R[9]);
            ImGui::Text("R10 %08x", NDS::ARM9->R[10]);
            ImGui::Text("R11 %08x", NDS::ARM9->R[11]);
            ImGui::Text("R12 %08x", NDS::ARM9->R[12]);
            ImGui::Text("SP  %08x", NDS::ARM9->R[13]);
            ImGui::Text("LR  %08x", NDS::ARM9->R[14]);
            ImGui::Text("PC  %08x", NDS::ARM9->R[15]);
            ImGui::End();
        }

        if (show_arm7_registers)
        {
            ImGui::Begin("ARM7", &show_arm7_registers);
            ImGui::Text("R0  %08x", NDS::ARM7->R[0]);
            ImGui::Text("R1  %08x", NDS::ARM7->R[1]);
            ImGui::Text("R2  %08x", NDS::ARM7->R[2]);
            ImGui::Text("R3  %08x", NDS::ARM7->R[3]);
            ImGui::Text("R4  %08x", NDS::ARM7->R[4]);
            ImGui::Text("R5  %08x", NDS::ARM7->R[5]);
            ImGui::Text("R6  %08x", NDS::ARM7->R[6]);
            ImGui::Text("R7  %08x", NDS::ARM7->R[7]);
            ImGui::Text("R8  %08x", NDS::ARM7->R[8]);
            ImGui::Text("R9  %08x", NDS::ARM7->R[9]);
            ImGui::Text("R10 %08x", NDS::ARM7->R[10]);
            ImGui::Text("R11 %08x", NDS::ARM7->R[11]);
            ImGui::Text("R12 %08x", NDS::ARM7->R[12]);
            ImGui::Text("SP  %08x", NDS::ARM7->R[13]);
            ImGui::Text("LR  %08x", NDS::ARM7->R[14]);
            ImGui::Text("PC  %08x", NDS::ARM7->R[15]);
            ImGui::End();
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