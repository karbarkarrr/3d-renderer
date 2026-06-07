#include "application.h"

#include <GLFW/glfw3.h>

namespace renderer {
    Application::Application(int width, int height)
        : window_(width, height, "3D-renderer"),
          screen_(width, height),
          controller_(window_.getHandle(), scene_.getCamera(), scene_.getLight()) {
        auto obj = kernel::Object::loadFromFile("assets/african_head.obj");
        obj.setPosition({0.0f, 0.0f, 0.0f});
        scene_.addObject(obj);
    }

    void Application::run() {
        while (!window_.shouldClose()) {
            window_.pollEvents();
            controller_.processInput(deltaTime());
            renderer_.render(scene_, screen_);
            window_.display(screen_);
        }
    }

    float Application::deltaTime() {
        static float pastTime = 0.0f;
        const auto curTime = static_cast<float>(glfwGetTime());
        const float dt = curTime - pastTime;
        pastTime = curTime;
        return std::min(dt, 0.05f);
    }
}
