#include "camera.h"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

namespace renderer::kernel {
    static constexpr Vector3 kGlobalUp = Vector3(0.0f, 1.0f, 0.0f);

    Camera::Camera(Vector3 position, float fov, float aspect, float nearPlane, float farPlane) : position_(position),
        fov_(fov), aspect_(aspect), near_(nearPlane), far_(farPlane) {
    };

    Matrix4 Camera::getViewMatrix() const {
        const Vector3 forward = getForward();
        return glm::lookAt(position_, position_ + forward, kGlobalUp);
    }

    Matrix4 Camera::getProjectionMatrix() const {
        return glm::perspective(glm::radians(fov_), aspect_, near_, far_);
    }

    Vector3 Camera::getPosition() const {
        return position_;
    }

    void Camera::moveForward(float delta) {
        const Vector3 forward = getForward();
        position_ += forward * delta;
    }

    void Camera::moveRight(float delta) {
        position_ += getRight() * delta;
    }

    void Camera::moveUp(float delta) {
        position_ += kGlobalUp * delta;
    }

    void Camera::rotate(float yawDelta, float pitchDelta) {
        yaw_ += yawDelta;

        pitch_ += pitchDelta;
        if (pitch_ > 89.0f) {
            pitch_ = 89.0f;
        }
        if (pitch_ < -89.0f) {
            pitch_ = -89.0f;
        }
    }

    Vector3 Camera::getForward() const {
        const float radYaw = glm::radians(yaw_);
        const float radPitch = glm::radians(pitch_);
        Vector3 forward;
        forward.x = std::cos(radPitch) * std::cos(radYaw);
        forward.y = std::sin(radPitch);
        forward.z = std::cos(radPitch) * std::sin(radYaw);
        return glm::normalize(forward);
    }

    Vector3 Camera::getRight() const {
        return glm::normalize(glm::cross(getForward(), kGlobalUp));
    }
}
