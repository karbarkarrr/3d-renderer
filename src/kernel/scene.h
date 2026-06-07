#pragma once

#include "camera.h"
#include "light.h"
#include "object.h"


namespace renderer::kernel {
    class Scene {
    public:
        Scene();
        void addObject(const Object &object);

        const std::vector<Object> &getObjects() const;
        Camera &getCamera();
        const Camera &getCamera() const;
        Light &getLight();
        const Light &getLight() const;

        Matrix4 getViewMatrix() const;
        Matrix4 getProjectionMatrix() const;

    private:
        std::vector<Object> objects_;
        Camera camera_;
        Light light_;
    };

}
