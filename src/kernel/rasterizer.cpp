#include "rasterizer.h"

#include <algorithm>

namespace {
    using namespace renderer::kernel;

    Vector3 applyLight(const Vector3 &N, const Vector3 &pos,
                       const Vector3 &color, const Light &light) {
        if (!light.on) {
            return color * light.ambient;
        }
        const Vector3 toLight = light.position - pos;
        const float d = glm::length(toLight);
        const float NL = glm::max(0.0f, glm::dot(glm::normalize(N), glm::normalize(toLight)));
        const float I = light.ambient + NL * light.intensity / (1.0f + 0.1f * d + 0.04f * d * d);
        return glm::clamp(color * I, Vector3(0.0f), Vector3(1.0f));
    }
}

namespace renderer::kernel {
    void rasterizeTriangle(const Vertex &v0, const Vertex &v1, const Vertex &v2,
                           Screen &screen, const Light &light, const Texture &texture) {
        auto [minX, maxX] = std::minmax({v0.position.x, v1.position.x, v2.position.x});
        auto [minY, maxY] = std::minmax({v0.position.y, v1.position.y, v2.position.y});

        int x0 = static_cast<int>(std::clamp(minX, 0.0f, static_cast<float>(screen.getWidth() - 1)));
        int x1 = static_cast<int>(std::clamp(maxX, 0.0f, static_cast<float>(screen.getWidth() - 1)));
        int y0 = static_cast<int>(std::clamp(minY, 0.0f, static_cast<float>(screen.getHeight() - 1)));
        int y1 = static_cast<int>(std::clamp(maxY, 0.0f, static_cast<float>(screen.getHeight() - 1)));

        float v0x = v0.position.x, v0y = v0.position.y;
        float v1x = v1.position.x, v1y = v1.position.y;
        float v2x = v2.position.x, v2y = v2.position.y;

        float denom = (v1y - v2y) * (v0x - v2x) + (v2x - v1x) * (v0y - v2y);
        if (std::abs(denom) < 1e-8f) {
            return;
        }
        const float faceSign = (denom < 0.0f) ? 1.0f : -1.0f;

        for (int y = y0; y <= y1; ++y) {
            float fy = static_cast<float>(y);
            for (int x = x0; x <= x1; ++x) {
                float fx = static_cast<float>(x);

                float w0 = ((v1y - v2y) * (fx - v2x) + (v2x - v1x) * (fy - v2y)) / denom;
                float w1 = ((v2y - v0y) * (fx - v2x) + (v0x - v2x) * (fy - v2y)) / denom;
                float w2 = 1.0f - w0 - w1;
                if (w0 < 0.0f || w1 < 0.0f || w2 < 0.0f) {
                    continue;
                }

                float depth = w0 * v0.position.z + w1 * v1.position.z + w2 * v2.position.z;

                float sumInvW = w0 * v0.invW + w1 * v1.invW + w2 * v2.invW;
                if (sumInvW < 1e-7f) {
                    continue;
                }
                Vector3 norm = glm::normalize(
                    (w0 * v0.normal * v0.invW + w1 * v1.normal * v1.invW + w2 * v2.normal * v2.invW) / sumInvW
                ) * faceSign;
                Vector3 wpos = (w0 * v0.worldPos * v0.invW + w1 * v1.worldPos * v1.invW + w2 * v2.worldPos * v2.invW) /
                               sumInvW;

                Vector3 col;
                if (texture.valid) {
                    Vector2 uv = (w0 * v0.uv * v0.invW + w1 * v1.uv * v1.invW + w2 * v2.uv * v2.invW) / sumInvW;
                    Vector3 vCol = (w0 * v0.color * v0.invW + w1 * v1.color * v1.invW + w2 * v2.color * v2.invW) /
                                   sumInvW;
                    col = texture.sample(uv.x, uv.y) * vCol;
                } else {
                    col = (w0 * v0.color * v0.invW + w1 * v1.color * v1.invW + w2 * v2.color * v2.invW) / sumInvW;
                }

                screen.setPixel(x, y, applyLight(norm, wpos, col, light), depth);
            }
        }
    }
}
