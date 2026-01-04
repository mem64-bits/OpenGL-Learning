#pragma once
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Camera.h"

enum ShaderState
{
    NORMAL,
    NEGATIVE,
    GREYSCALE
};

struct WindowState
{
    core::Camera* pCamera = nullptr;
    float lastX = 0.0f;
    float lastY = 0.0f;
    ShaderState shaderState = NORMAL;
    bool wireframe = false;
    bool firstMouse = true;
};

// automatically resizes the viewport when the window size is changed
inline void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


inline void key_callback(GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action,
                         [[maybe_unused]] int mods)
{
    // Single casting and safety check at the top
    auto* state = static_cast<WindowState *>( glfwGetWindowUserPointer(window) );
    if(!state) return;

    if(action == GLFW_PRESS)
    {
        if(key == GLFW_KEY_SPACE)
        {
            state->wireframe = !state->wireframe;
            glPolygonMode(GL_FRONT_AND_BACK, state->wireframe ? GL_LINE : GL_FILL);
        } else if(key == GLFW_KEY_1)
        {
            state->shaderState = NORMAL;
        } else if(key == GLFW_KEY_2)
        {
            state->shaderState = GREYSCALE;
        } else if(key == GLFW_KEY_3)
        {
            state->shaderState = NEGATIVE;
        }
    }
}

// Checks for input every frame in render loop
inline void processInput(GLFWwindow* window, core::Camera& camera, const float deltaTime)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(core::FORWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(core::LEFT, deltaTime);

    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(core::BACKWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(core::RIGHT, deltaTime);

    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        camera.reset();
}


inline void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    auto* state = static_cast<WindowState *>( glfwGetWindowUserPointer(window) );
    if(!state || !state->pCamera) return;// Safety check

    const auto xpos = static_cast<float>( xposIn );
    const auto ypos = static_cast<float>( yposIn );

    if(state->firstMouse)
    {
        state->lastX = xpos;
        state->lastY = ypos;
        state->firstMouse = false;
        return;
    }

    float xoffset = xpos - state->lastX;
    float yoffset = state->lastY - ypos;// reversed since y-coordinates go from bottom to top

    state->lastX = xpos;
    state->lastY = ypos;
    state->pCamera->ProcessMouseMovement(xoffset, yoffset);
}


inline void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    const auto* state = static_cast<WindowState *>( glfwGetWindowUserPointer(window) );
    if(!state || !state->pCamera) return;// Safety check
    state->pCamera->ProcessMouseScroll(static_cast<float>( yoffset ));
}
