#pragma once

#include <GLFW/glfw3.h>
#include "kernel/screen.h"

namespace renderer::view {
    class Window {
    public:
        Window(int width, int height, const char *title);

        ~Window();

        bool shouldClose() const;

        void display(const kernel::Screen &screen) const;

        void pollEvents() const;

        GLFWwindow *getHandle() const;

    private:
        void initGL();

        GLFWwindow *handle_;
        unsigned int texture_{};
        unsigned int vao_{}, vbo_{}, program_{};
    };
}
