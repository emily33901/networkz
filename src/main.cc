#include <cstdio>

#include "render/render.hh"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <imgui.h>

int main() {
    printf(IMGUI_VERSION "\n");

    if (!render::init()) {
        printf("Unable to init render\n");
    }

    // Set our loop function first because its needed later on
    emscripten_set_main_loop(render::loop, 0, true);
}