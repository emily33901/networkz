#include "render.hh"

#include "common/platform.hh"

#include "console/console.hh"
#include "test/test.hh"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

#include <SDL.h>
#include <SDL_opengles2.h>

#endif

#include "imgui.h"
#include "imgui/opengl.hh"
#include "imgui/platform.hh"

#include "imgui_node_editor.h"

// TODO cleanup

namespace ed = ax::NodeEditor;

namespace render {

extern SDL_Window *  window;
extern SDL_GLContext glContext;

// Our state (make them static = more or less global) as a convenience to keep the example terse.
static bool   show_demo_window    = true;
static bool   show_another_window = false;
static ImVec4 clear_color         = ImVec4(0.1f, 0.1f, 0.11f, 1.00f);

void ProcessEvents() {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        // Capture events here, based on io.WantCaptureMouse and io.WantCaptureKeyboard
    }
}

void BeginFrame() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);

    ProcessEvents();

    ImGui::NewFrame();
}

void EndFrame() {

    ImGuiIO &io = ImGui::GetIO();

    // Rendering
    auto ratio = emscripten_get_device_pixel_ratio();

    SDL_SetWindowSize(window, render::WindowWidth() * ratio, render::WindowHeight() * ratio);
    //resizeCanvas();

    ImGui::Render();
    SDL_GL_MakeCurrent(window, glContext);
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}

void Frame() {

    BeginFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f       = 0.0f;
        static int   counter = 0;

        ImGui::Begin("Hello, networkz!"); // Create a window called "Hello, world!" and append into it.
        Defer(ImGui::End());

        ImGui::Text("This is some useful text.");          // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float *)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    // 3. Show another simple window.
    if (show_another_window) {
        ImGui::Begin("Another Window", &show_another_window);
        Defer(ImGui::End());
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;

        static ed::EditorContext *context = ed::CreateEditor();
        ed::SetCurrentEditor(context);

        ed::Begin("My Editor", {500, 500});

        int uniqueId = 1;

        // Start drawing nodes.
        ed::BeginNode(uniqueId++);
        ImGui::Text("Node A");
        ed::BeginPin(uniqueId++, ed::PinKind::Input);
        ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ed::BeginPin(uniqueId++, ed::PinKind::Output);
        ImGui::Text("Out ->");
        ed::EndPin();
        ed::EndNode();

        ed::End();

        ed::SetCurrentEditor(nullptr);
    }

    test::Frame();
    console::Frame();

    EndFrame();
}
} // namespace render