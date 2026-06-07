#pragma once

#include "../linalg.h"

namespace renderer::kernel {
    class Camera {
    public:
        Camera(Vector3 position, float fov, float aspect, float nearPlane, float farPlane);

        Matrix4 getViewMatrix() const;
        Matrix4 getProjectionMatrix() const;
        Vector3 getForward() const;
        Vector3 getPosition() const;

        void moveForward(float delta);
        void moveRight(float delta);
        void moveUp(float delta);
        void rotate(float yawDelta, float pitchDelta);

    private:
        Vector3 position_;
        float yaw_ = -90.0f;
        float pitch_ = 0.0f;
        float aspect_, near_, far_;
        float fov_ = 45.0f;

        Vector3 getRight() const;
    };
}
