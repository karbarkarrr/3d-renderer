#include "scene.h"

namespace renderer::kernel {
    Scene::Scene() : camera_(Camera{
        Vector3(0.0f, 0.0f, 5.0f),
        60.0f, 800.0f / 600.0f, 0.1f, 100.0f
    }) {
    };

    void Scene::addObject(const Object &object) {
        objects_.push_back(object);
    }

    const std::vector<Object> &Scene::getObjects() const {
        return objects_;
    }
    Camera &Scene::getCamera() {
        return camera_;
    }
    const Camera &Scene::getCamera() const {
        return camera_;
    }
    Light &Scene::getLight() {
        return light_;
    }
    const Light &Scene::getLight() const {
        return light_;
    }
    Matrix4 Scene::getViewMatrix() const {
        return camera_.getViewMatrix();
    }
    Matrix4 Scene::getProjectionMatrix() const {
        return camera_.getProjectionMatrix();
    }

}
