#pragma once

#include "scene.h"
#include "screen.h"

namespace renderer::kernel {
    class Renderer {
    public:
        void render(const Scene &scene, Screen &screen);
    };
}