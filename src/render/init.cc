#include "common/platform.hh"

#include "filesystem/filesystem.hh"
#include "render.hh"

// TODO cleanup this file

#if platform_emscripten()
#include <emscripten.h>

#include <SDL.h>
#include <SDL_opengles2.h>

#endif

#include "imgui.h"
#include "imgui/opengl.hh"
#include "imgui/platform.hh"

#include "imgui_node_editor.h"

namespace ed = ax::NodeEditor;

namespace render {

// Emscripten requires to have full control over the main loop. We're going to store our SDL book-keeping variables globally.
// Having a single function that acts as a loop prevents us to store state in the stack of said function. So we need some location for this.
SDL_Window *  window    = NULL;
SDL_GLContext glContext = NULL;

Option<SDL_Window *> CreateWindow() {
    // Try to init SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return None();
    }

    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);

    auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    auto window       = SDL_CreateWindow("Networkz", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);

    return Some(window);
}

Option<SDL_GLContext> CreateGLContext(SDL_Window *window) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    auto context = SDL_GL_CreateContext(window);

    if (context != nullptr) return Some(context);

    fprintf(stderr, "Failed to initialize WebGL context!\n");

    return None();
}

bool Init() {
    // Setup SDL
    window = CreateWindow().value();

    // For the browser using Emscripten, we are going to use WebGL1 with GL ES2. See the Makefile. for requirement details.
    // It is very likely the generated file won't work in many browsers. Firefox is the only sure bet, but I have successfully
    // run this code on Chrome for Android for example.
    auto glsl_version = "#version 100";
    //const char* glsl_version = "#version 300 es";

    glContext = CreateGLContext(window).value();

    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    // TODO this will only apply for a non-emscripten build
    // We should also try to find some way to capture this input
    // before the browser tabs away - becuase this navication is
    // very useful
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

    // TODO save this into local browser storage
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = NULL;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Fonts loading
#ifndef IMGUI_DISABLE_FILE_FUNCTIONS
    auto loadFont = [&io](const char *path) {
        filesystem::Request(path, [io](auto path, auto text) {
            if (text == None()) {
                printf("Unable to load font %s\n", path.c_str());
            } else {
                io.Fonts->AddFontFromMemoryTTF((void *)text->c_str(), text->size(), 17);

                ImGui_ImplOpenGL3_CreateFontsTexture();
            }
        });
    };
    // TODO load fonts from memory results from a fetch()
    // These should probably be got from a google fonts cdn or similar
    loadFont("https://cdn.jsdelivr.net/npm/roboto-mono-webfont@2.0.986/fonts/RobotoMono-Regular.ttf");
    // loadFont("https://cdn.jsdelivr.net/npm/roboto-mono-webfont@2.0.986/fonts/RobotoMono-Thin.ttf");
    // loadFont("https://cdn.jsdelivr.net/npm/roboto-mono-webfont@2.0.986/fonts/RobotoMono-Bold.ttf");
#endif

    return true;
}
} // namespace render
