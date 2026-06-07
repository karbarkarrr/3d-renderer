#include "window.h"

namespace renderer::view {
    const char *kVertexShaderSource =
        "#version 330 core\n"
        "layout(location = 0) in vec2 pos;\n"
        "layout(location = 1) in vec2 uv;\n"
        "out vec2 vUV;\n"
        "void main() {\n"
        "    gl_Position = vec4(pos, 0.0, 1.0);\n"
        "    vUV = uv;\n"
        "}\n";

    const char *kFragmentShaderSource =
        "#version 330 core\n"
        "in vec2 vUV;\n"
        "out vec4 color;\n"
        "uniform sampler2D uTex;\n"
        "void main() {\n"
        "    color = texture(uTex, vUV);\n"
        "}\n";

    Window::Window(int width, int height, const char *title) {
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (window == nullptr) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }
        glfwMakeContextCurrent(window);
        initGL();
        handle_ = window;
    }

    Window::~Window() {
        glfwTerminate();
    }

    bool Window::shouldClose() const {
        return glfwWindowShouldClose(handle_);
    }

    void Window::display(const kernel::Screen &screen) const {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program_);
        glBindTexture(GL_TEXTURE_2D, texture_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen.getWidth(), screen.getHeight(), 0,
                     GL_RGB, GL_UNSIGNED_BYTE, screen.getPixels().data());
        glBindVertexArray(vao_);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void Window::pollEvents() const {
        glfwSwapBuffers(handle_);
        glfwPollEvents();
    }

    GLFWwindow *Window::getHandle() const {
        return handle_;
    }

    void Window::initGL() {
        float vertices[] = {
            -1.0f, -1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 0.0f
        };

        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);
        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                              nullptr);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                              reinterpret_cast<void *>(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &kVertexShaderSource, nullptr);
        glCompileShader(vertexShader);

        const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &kFragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);

        program_ = glCreateProgram();
        glAttachShader(program_, vertexShader);
        glAttachShader(program_, fragmentShader);
        glLinkProgram(program_);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        glGenTextures(1, &texture_);
        glBindTexture(GL_TEXTURE_2D, texture_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
}
