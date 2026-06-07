#pragma once

#include "../linalg.h"
#include <vector>
#include <cmath>
#include <cstdint>

namespace renderer::kernel {
    struct Texture {
        static constexpr float kByteNorm = 1.0f / 255.0f;

        std::vector<uint8_t> pixels;
        int width = 0;
        int height = 0;
        bool valid = false;

        Vector3 sample(float U, float V) const {
            const int col = std::min(static_cast<int>((U - std::floor(U)) * width), width - 1);
            const int row = std::min(static_cast<int>((1.0f - (V - std::floor(V))) * height), height - 1);
            const int offset = (row * width + col) * 3;
            return {
                pixels[offset] * kByteNorm,
                pixels[offset + 1] * kByteNorm,
                pixels[offset + 2] * kByteNorm
            };
        }
    };
}
