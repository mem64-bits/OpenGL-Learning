#pragma once
#include <glad/gl.h>
#include <GLFW/glfw3.h>


enum ShaderState
{
    NORMAL,
    NEGATIVE,
    GREYSCALE
};

struct WindowState
{
    bool wireframe = false;
    ShaderState shaderState = NORMAL;
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

    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        auto *s = static_cast<WindowState *>(glfwGetWindowUserPointer(window));
        if (!s) return;
        s->shaderState = NORMAL;
    }

    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        auto *s = static_cast<WindowState *>(glfwGetWindowUserPointer(window));
        if (!s) return;
        s->shaderState = GREYSCALE;
    }

    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        auto* s = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
        if (!s) return;
        s->shaderState = NEGATIVE;
    }
}

// Checks for input every frame in render loop
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

