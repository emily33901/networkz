#pragma once

namespace render {

// Singleton for rendering

extern "C" int   CanvasWidth();
extern "C" int   CanvasHeight();
extern "C" int   WindowWidth();
extern "C" int   WindowHeight();
extern "C" float DevicePixelRatio();

bool Init();

void Frame();
} // namespace render