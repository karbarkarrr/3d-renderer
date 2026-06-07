#pragma once

#include "light.h"
#include "primitive.h"
#include "screen.h"
#include "texture.h"

namespace renderer::kernel {
    void rasterizeTriangle(const Vertex &v0, const Vertex &v1, const Vertex &v2,
                           Screen &screen, const Light &light, const Texture &texture);
}
