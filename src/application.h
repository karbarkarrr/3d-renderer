#pragma once

#include "controller/controller.h"
#include "kernel/renderer.h"
#include "kernel/scene.h"
#include "kernel/screen.h"
#include "view/window.h"

namespace renderer {
    class Application {
    public:
        Application(int width, int height);
        void run();

    private:
        static float deltaTime();

        view::Window window_;
        kernel::Scene scene_;
        kernel::Renderer renderer_;
        kernel::Screen screen_;
        controller::Controller controller_;
    };
}
