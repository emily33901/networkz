#include <cstdio>

#include "console/console.hh"
#include "filesystem/filesystem.hh"
#include "render/render.hh"
#include "test/test.hh"

#include "connections/connections.hh"
#include "main.hh"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <imgui.h>

#include "common/toml.hh"

static void Frame() {
    render::Frame();
    filesystem::Frame();
}

int main() {
    filesystem::Request("/application.toml", [](auto path, auto text) {
        if (text == None()) {
            printf("Unable to load application.toml!\n");
        } else {
            auto applicationToml = cpptoml::parse_text(*text);

            auto name        = applicationToml->template get_as<std::string>("name").value_or("");
            auto version     = applicationToml->template get_as<std::string>("version").value_or("");
            auto compileDate = applicationToml->template get_as<std::string>("compile_date").value_or("");

            printf("%s v%s compiled at %s\n", name.c_str(), version.c_str(), compileDate.c_str());
        }
    });
    printf(IMGUI_VERSION "\n");

    bool success = true;

#define InitSubsystem(x)                   \
    if (!x::Init()) {                      \
        printf("Unable to load " #x "\n"); \
        success = false;                   \
    }

    InitSubsystem(filesystem);
    InitSubsystem(render);
    InitSubsystem(test);
    InitSubsystem(node::factory);
    InitSubsystem(console);

    workspace::globalWorkspace = new workspace::Workspace();

    if (success) {
        // Set our loop function first because its needed later on
        emscripten_set_main_loop(Frame, 0, true);
    }
}

// Temp global state stuff
workspace::Workspace *workspace::globalWorkspace;