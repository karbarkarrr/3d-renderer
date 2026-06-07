#pragma once
#include "../linalg.h"

namespace renderer::kernel {
    struct Vertex {
        Vector3 position;
        Vector3 worldPos;
        Vector3 normal;
        Vector2 uv;
        Vector3 color;
        float invW = 1.0f;
    };

    struct Triangle {
        int v0, v1, v2;
    };
}
