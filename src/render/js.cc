#include "common/platform.hh"

#include "render.hh"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

#include <SDL.h>
#include <SDL_opengles2.h>

#endif

namespace render {

// These functions are used to get values from the js
// To give to the renderer

// TODO add version for non-emscripten builds

#if platform_emscripten() == false
#error Use emscripten to compile please
#endif

#if platform_emscripten()
EM_JS(int, CanvasWidth, (), {
    return Module.canvas.width;
});

EM_JS(int, CanvasHeight, (), {
    return Module.canvas.height;
});

EM_JS(int, WindowWidth, (), {
    return window.innerWidth;
});

EM_JS(int, WindowHeight, (), {
    return window.innerHeight;
});

EM_JS(float, DevicePixelRatio, (), {
    return window.devicePixelRatio;
})

EM_JS(void, ResizeCanvas, (), {
    _resizeCanvas();
});
#endif
} // namespace render