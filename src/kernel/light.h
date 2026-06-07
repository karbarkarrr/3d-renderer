#pragma once
#include "../linalg.h"

namespace renderer::kernel {
    struct Light {
        Vector3 position = {0.0f, 3.0f, 5.0f};
        float ambient = 0.15f;
        float intensity = 1.0f;
        bool on = false;
    };
}
