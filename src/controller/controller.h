#pragma once

#include <GLFW/glfw3.h>
#include "kernel/camera.h"
#include "kernel/light.h"

namespace renderer::controller {
    class Controller {
    public:
        explicit Controller(GLFWwindow *window, kernel::Camera &camera, kernel::Light &light);

        void processInput(float deltaTime);

    private:
        GLFWwindow *window_;
        kernel::Camera &camera_;
        kernel::Light &light_;

        float lastX_ = 400.0f;
        float lastY_ = 300.0f;
        bool firstMouse_ = true;
        bool lPressed_ = false;
        float sensitivity_ = 0.05f;
        float speed_ = 3.0f;
    };
}
