#include "renderer.h"
#include "rasterizer.h"

#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

namespace {
    using namespace renderer::kernel;

    struct ClipVert {
        Vector4 clip;
        Vector3 worldPos;
        Vector3 normal;
        Vector2 uv;
        Vector3 color;
    };

    ClipVert lerpVerts(const ClipVert &v0, const ClipVert &v1) {
        float t = (v0.clip.z + v0.clip.w) / (v0.clip.z + v0.clip.w - (v1.clip.z + v1.clip.w));
        return {
            glm::mix(v0.clip, v1.clip, t),
            glm::mix(v0.worldPos, v1.worldPos, t),
            glm::mix(v0.normal, v1.normal, t),
            glm::mix(v0.uv, v1.uv, t),
            glm::mix(v0.color, v1.color, t)
        };
    }

    std::vector<ClipVert> clipNear(const std::vector<ClipVert> &poly) {
        std::vector<ClipVert> out;
        out.reserve(poly.size() + 1);
        for (size_t i = 0; i < poly.size(); ++i) {
            const ClipVert &prev = poly[(i + poly.size() - 1) % poly.size()];
            const ClipVert &cur = poly[i];

            bool prevIn = prev.clip.z >= -prev.clip.w;
            bool curIn = cur.clip.z >= -cur.clip.w;
            if (prevIn != curIn) {
                out.push_back(lerpVerts(prev, cur));
            }
            if (curIn) {
                out.push_back(cur);
            }
        }
        return out;
    }

    Vertex toScreen(const ClipVert &cv, int width, int height) {
        Vector3 ndc = Vector3(cv.clip) / cv.clip.w;
        return {
            {
                (ndc.x + 1.0f) * width / 2.0f,
                (1.0f - ndc.y) * height / 2.0f,
                ndc.z
            },
            cv.worldPos,
            glm::normalize(cv.normal),
            cv.uv,
            cv.color,
            1.0f / cv.clip.w
        };
    }
}

namespace renderer::kernel {
    void Renderer::render(const Scene &scene, Screen &screen) {
        screen.clear();
        Matrix4 view = scene.getViewMatrix();
        Matrix4 projection = scene.getProjectionMatrix();
        int weight = screen.getWidth();
        int height = screen.getHeight();

        for (const Object &obj: scene.getObjects()) {
            Matrix4 model = obj.getModelMatrix();
            Matrix4 mvp = projection * view * model;
            Matrix3 normalMat = Matrix3(glm::transpose(glm::inverse(model)));

            const auto &verts = obj.getVertices();
            const auto &faces = obj.getFaces();
            const Texture &texture = obj.getTexture();
            const Light &light = scene.getLight();

            tbb::parallel_for(
                tbb::blocked_range<size_t>(0, faces.size()),
                [&](const tbb::blocked_range<size_t> &range) {
                    for (size_t fi = range.begin(); fi != range.end(); ++fi) {
                        const Triangle &tri = faces[fi];
                        const Vertex &v0 = verts[tri.v0];
                        const Vertex &v1 = verts[tri.v1];
                        const Vertex &v2 = verts[tri.v2];

                        Vector3 wp0 = Vector3(model * Vector4(v0.position, 1.0f));
                        Vector3 wp1 = Vector3(model * Vector4(v1.position, 1.0f));
                        Vector3 wp2 = Vector3(model * Vector4(v2.position, 1.0f));

                        auto polygon = clipNear({
                            {
                                mvp * Vector4(v0.position, 1.0f), wp0, normalMat * v0.normal,
                                v0.uv, v0.color
                            },
                            {
                                mvp * Vector4(v1.position, 1.0f), wp1, normalMat * v1.normal,
                                v1.uv, v1.color
                            },
                            {
                                mvp * Vector4(v2.position, 1.0f), wp2, normalMat * v2.normal,
                                v2.uv, v2.color
                            }
                        });
                        if (polygon.size() < 3) {
                            continue;
                        }

                        Vertex s0 = toScreen(polygon[0], weight, height);
                        for (size_t i = 1; i + 1 < polygon.size(); ++i) {
                            rasterizeTriangle(s0,
                                              toScreen(polygon[i], weight, height),
                                              toScreen(polygon[i + 1], weight, height),
                                              screen, light, texture);
                        }
                    }
                }
            );
        }
    }
}
