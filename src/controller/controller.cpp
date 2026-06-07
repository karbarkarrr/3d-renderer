#include "controller.h"

#include <algorithm>

namespace renderer::controller {
    Controller::Controller(GLFWwindow *window, kernel::Camera &camera, kernel::Light &light) : window_(window),
        camera_(camera), light_(light) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (glfwRawMouseMotionSupported()) {
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
    }

    void Controller::processInput(float deltaTime) {
        if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
            camera_.moveForward(speed_ * deltaTime);
        }
        if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
            camera_.moveForward(-speed_ * deltaTime);
        }
        if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
            camera_.moveRight(-speed_ * deltaTime);
        }
        if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
            camera_.moveRight(speed_ * deltaTime);
        }
        if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS) {
            camera_.moveUp(-speed_ * deltaTime);
        }
        if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS) {
            camera_.moveUp(speed_ * deltaTime);
        }
        if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window_, true);
        }

        const bool lDown = glfwGetKey(window_, GLFW_KEY_L) == GLFW_PRESS;
        if (lDown && !lPressed_) {
            light_.on = !light_.on;
            if (light_.on) {
                light_.position = camera_.getPosition();
            }
        }
        lPressed_ = lDown;

        if (glfwGetKey(window_, GLFW_KEY_EQUAL) == GLFW_PRESS) {
            light_.intensity = std::min(light_.intensity + deltaTime, 2.0f);
        }
        if (glfwGetKey(window_, GLFW_KEY_MINUS) == GLFW_PRESS) {
            light_.intensity = std::max(light_.intensity - deltaTime, 0.0f);
        }

        double xpos, ypos;
        glfwGetCursorPos(window_, &xpos, &ypos);

        if (firstMouse_) {
            lastX_ = static_cast<float>(xpos);
            lastY_ = static_cast<float>(ypos);
            firstMouse_ = false;
            return;
        }

        constexpr float kMaxDelta = 8.0f;
        const float dx = std::clamp((static_cast<float>(xpos) - lastX_) * sensitivity_, -kMaxDelta, kMaxDelta);
        const float dy = std::clamp(-(static_cast<float>(ypos) - lastY_) * sensitivity_, -kMaxDelta, kMaxDelta);
        lastX_ = static_cast<float>(xpos);
        lastY_ = static_cast<float>(ypos);
        camera_.rotate(dx, dy);
    }
}
