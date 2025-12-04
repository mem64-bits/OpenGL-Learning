#pragma once
#include <glad/gl.h>
#include <GLFW/glfw3.h>

struct WindowState
{
    bool normal = true;
    bool wireframe = false;
    bool negative = false;
    bool greyscale = false;
    bool transitioning = false;
    float transStartTime = 0.0f;
};

// automatically resizes the viewport when the window size is changed
void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void key_callback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        auto *s = static_cast<WindowState *>(glfwGetWindowUserPointer(window));
        if (!s) return;
        s->wireframe = !s->wireframe;
        glPolygonMode(GL_FRONT_AND_BACK, s->wireframe ? GL_LINE : GL_FILL);
    }

    //if (key == GLFW_KEY_N && action == GLFW_PRESS) {
    //    auto *s = static_cast<WindowState *>(glfwGetWindowUserPointer(window));
    //    if (!s) return;
    //    s->negative = !s->negative;
    //}

    //if (key == GLFW_KEY_G && action == GLFW_PRESS) {
    //    auto *s = static_cast<WindowState *>(glfwGetWindowUserPointer(window));
    //    if (!s) return;
    //    s->greyscale = !s->greyscale;
    //}

    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        auto *s = static_cast<WindowState *>(glfwGetWindowUserPointer(window));
        if (!s) return;
        s->transStartTime = static_cast<float>(glfwGetTime());
        s->transitioning = !s->transitioning;
        s->negative = !s->negative;
    }
}

// Checks for input every frame in render loop
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

