#include <glad/gl.h>
#include  <GLFW/glfw3.h>
#include "glfwHelpers.h"
#include "Shader.h"
#include "Texture.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"

#include "Window.h"
#include "FPSCounter.h"
#include <imgui.h>


int main()
{
    Window window({.vSync = true});
    glfwSetInputMode(window.getGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window.getGLFWWindow(), framebuffer_size_callback);

    WindowState state{};
    state.lastX = static_cast<float>(window.getFramebufferWidth()) / 2.0f;
    state.lastY = static_cast<float>(window.getFramebufferHeight()) / 2.0f;
    glfwSetWindowUserPointer(window.getGLFWWindow(), &state);

    glfwSetKeyCallback(window.getGLFWWindow(), key_callback);
    glfwSetCursorPosCallback(window.getGLFWWindow(), mouse_callback);
    glfwSetScrollCallback(window.getGLFWWindow(), scroll_callback);

    // Uses our new shader object cutting down on boilerplate code
    Shader shader{"assets/shaders/shader.vert", "assets/shaders/shader.frag"};

    // Vertex data needed for a 3D Cube Object (Using modern std::vector<>)
    const std::vector<float> vertices = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
    };

    unsigned int VAO;
    unsigned int VBO;
    const Texture tex = Texture("assets/textures/SugarCoat.jpg");

    // Creates Buffers need for VAO,VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Saves buffer config to VAO
    glBindVertexArray(VAO);

    // VBO setup
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Sends texture data to frag shader using uniforms
    shader.setUniform<int>("tex", 0);
    shader.use();


    /* Defines our Camera class to automatically change our view and perspective
     * matrices to simulate a camera */
    Camera camera({.Pos = glm::vec3(0.0f, 0.0f, 5.0f), .Speed = 7.5f, .MouseSens = 0.1f});
    state.pCamera = &camera;

    // For 3D rendering we need to enable the z buffer
    glEnable(GL_DEPTH_TEST);

    // Enables true transparency for images
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // We can now define cubes positions anywhere in the global world space
    const std::vector<glm::vec3> cubePositions = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    FPSCounter fps;
    window.setClearColour(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
    while (!window.shouldClose())
    {
        window.updateTime();
        fps.update(window.getDeltaTime());
        processInput(window.getGLFWWindow(), camera, window.getDeltaTime());
        window.clear();
        // Start the ImGui frame
        window.beginImgui();
        fps.drawUI();

        shader.use();
        tex.bindTexture(0);
        shader.setUniform<int>("shaderState", state.shaderState);
        glBindVertexArray(VAO);

        glm::mat4 view = camera.getViewMatrix();
        shader.setUniform<glm::mat4>("view", view);

        const glm::mat4 projection = camera.getProjectionMatrix(window.getFramebufferWidth(),
                                                                window.getFramebufferHeight());
        shader.setUniform<glm::mat4>("projection", projection);

        for (unsigned int i = 0; i < std::size(cubePositions); i++)
        {
            /* We need to update the model matrix for each cube every frame
             * as transformations need to be updated every frame*/
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);

            if (i % 2 == 0)
            {
                model = glm::rotate(model, static_cast<float>(window.getWindowTime()),
                                    glm::vec3(1.0f, 1.0f, 1.0f));
            }
            if (i % 2 != 0)
            {
                model = glm::rotate(model, static_cast<float>(window.getWindowTime()), glm::vec3(0.0f, -1.0f, 0.0f));
            }

            const float rotateAngle = 20.0f * static_cast<float>(i);
            model = glm::rotate(model, rotateAngle, glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setUniform<glm::mat4>("model", model);

            // A cube has 36 vertices (6 faces * 2 triangles * 3 vertices each)
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        window.endImgui();
        window.swapBuffers();
        window.pollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

