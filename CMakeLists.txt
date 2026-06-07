cmake_minimum_required(VERSION 3.10)
project(3d_renderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)
find_package(TBB REQUIRED)

add_executable(renderer
        src/main.cpp
        src/application.cpp
        src/controller/controller.cpp
        src/kernel/camera.cpp
        src/kernel/object.cpp
        src/kernel/rasterizer.cpp
        src/kernel/renderer.cpp
        src/kernel/scene.cpp
        src/kernel/screen.cpp
        src/view/window.cpp
)

target_compile_definitions(renderer PRIVATE
        GL_SILENCE_DEPRECATION
        GLFW_INCLUDE_GLCOREARB
)

target_include_directories(renderer PRIVATE src)

target_link_libraries(renderer
        OpenGL::GL
        glfw
        glm::glm
        assimp::assimp
        TBB::tbb
)