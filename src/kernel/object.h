#pragma once

#include <string>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "../linalg.h"
#include "primitive.h"
#include "texture.h"

namespace renderer::kernel {
    static constexpr Vector3 kWhiteColor = {1.0f, 1.0f, 1.0f};

    class Object {
    public:
        Matrix4 getModelMatrix() const;

        const std::vector<Vertex> &getVertices() const;

        const std::vector<Triangle> &getFaces() const;

        const Texture &getTexture() const;

        void setPosition(Vector3);

        void setScale(Vector3);

        void setRotation(Vector3);

        void setColor(Vector3);

        static Object loadFromFile(const std::string &path);

    private:
        Vector3 position_ = {0.0f, 0.0f, 0.0f};
        Vector3 scale_ = {1.0f, 1.0f, 1.0f};
        Vector3 rotation_ = {0.0f, 0.0f, 0.0f};
        Vector3 color_ = kWhiteColor;

        std::vector<Triangle> faces_;
        std::vector<Vertex> vertices_;
        Texture texture_;
    };
}
