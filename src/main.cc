#include <cstdio>

#include "filesystem/filesystem.hh"
#include "render/render.hh"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <imgui.h>

int main() {
    printf(IMGUI_VERSION "\n");

    bool success = true;

    if (!filesystem::Init()) {
        printf("Unable to load filesystem\n");

        success = false;
    }

    if (!render::Init()) {
        printf("Unable to init render\n");

        success = false;
    }

    if (success) {
        // Set our loop function first because its needed later on
        emscripten_set_main_loop(render::Frame, 0, true);
    }
}