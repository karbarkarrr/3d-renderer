#include "object.h"

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdexcept>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include "../linalg.h"

namespace renderer::kernel {
    static constexpr Vector3 kDefaultNormal = {0.0f, 1.0f, 0.0f};
    static constexpr Vector2 kDefaultUV = {0.0f, 0.0f};

    Matrix4 Object::getModelMatrix() const {
        auto model = Matrix4(1.0f);
        model = glm::translate(model, position_);
        model = glm::rotate(model, glm::radians(rotation_.x), Vector3(1, 0, 0));
        model = glm::rotate(model, glm::radians(rotation_.y), Vector3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rotation_.z), Vector3(0, 0, 1));
        model = glm::scale(model, scale_);
        return model;
    }

    const std::vector<Vertex> &Object::getVertices() const {
        return vertices_;
    }

    const std::vector<Triangle> &Object::getFaces() const {
        return faces_;
    }

    const Texture &Object::getTexture() const {
        return texture_;
    }

    void Object::setPosition(Vector3 pos) {
        position_ = pos;
    }

    void Object::setScale(Vector3 scale) {
        scale_ = scale;
    }

    void Object::setRotation(Vector3 rotation) {
        rotation_ = rotation;
    }

    void Object::setColor(Vector3 color) {
        color_ = color;
        for (auto &vert: vertices_) {
            vert.color = color;
        }
    }

    static Texture loadTexture(const std::string &dir, const std::string &name) {
        Texture result;
        if (name.empty()) {
            return result;
        }

        namespace fs = std::filesystem;
        fs::path texPath = fs::path(dir) / name;
        if (!fs::exists(texPath)) {
            texPath = fs::path(dir) / fs::path(name).filename();
        }

        int width, height, channels;
        unsigned char *data = stbi_load(texPath.string().c_str(), &width, &height, &channels, 3);
        if (!data) {
            return result;
        }

        result.width = width;
        result.height = height;
        result.pixels.assign(data, data + width * height * 3);
        result.valid = true;
        stbi_image_free(data);
        return result;
    }

    Object Object::loadFromFile(const std::string &path) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate);

        if (!scene || !scene->mRootNode) {
            throw std::runtime_error("Failed to load model: " + path);
        }

        const std::string dir = std::filesystem::path(path).parent_path().string();

        Object object;
        for (unsigned int mi = 0; mi < scene->mNumMeshes; ++mi) {
            aiMesh *mesh = scene->mMeshes[mi];
            const int base = static_cast<int>(object.vertices_.size());

            Vector3 color = kWhiteColor;
            Texture tex;

            if (mesh->mMaterialIndex < scene->mNumMaterials) {
                aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
                aiString texName;
                if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &texName) == AI_SUCCESS) {
                    tex = loadTexture(dir, texName.C_Str());
                }
                if (!tex.valid) {
                    aiColor3D diffuse;
                    if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS) {
                        const float brightness = diffuse.r + diffuse.g + diffuse.b;
                        const bool nearWhite = diffuse.r > 0.98f && diffuse.g > 0.98f && diffuse.b > 0.98f;
                        if (brightness > 0.01f && !nearWhite) {
                            color = {diffuse.r, diffuse.g, diffuse.b};
                        }
                    }
                }
            }

            if (tex.valid && !object.texture_.valid) {
                object.texture_ = std::move(tex);
            }

            for (unsigned int vi = 0; vi < mesh->mNumVertices; ++vi) {
                Vertex vert{};
                vert.position = {mesh->mVertices[vi].x, mesh->mVertices[vi].y, mesh->mVertices[vi].z};
                vert.normal = mesh->HasNormals()
                                  ? Vector3{mesh->mNormals[vi].x, mesh->mNormals[vi].y, mesh->mNormals[vi].z}
                                  : kDefaultNormal;
                vert.uv = mesh->mTextureCoords[0]
                              ? Vector2{mesh->mTextureCoords[0][vi].x, mesh->mTextureCoords[0][vi].y}
                              : kDefaultUV;
                vert.color = color;
                object.vertices_.push_back(vert);
            }

            for (unsigned int fidx = 0; fidx < mesh->mNumFaces; ++fidx) {
                const aiFace &face = mesh->mFaces[fidx];
                if (face.mNumIndices == 3) {
                    object.faces_.push_back({
                        base + static_cast<int>(face.mIndices[0]),
                        base + static_cast<int>(face.mIndices[1]),
                        base + static_cast<int>(face.mIndices[2])
                    });
                }
            }
        }
        return object;
    }
}
