#include <glad/gl.h>
#include  <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include <Camera.h>
#include <Window.h>
#include <FPSCounter.h>
#include <glfwHelpers.h>
#include <Shader.h>
#include <Texture.h>


int main()
{
    core::Window window({ .name = "Colours", .vSync = true });
    glfwSetInputMode(window.getGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window.getGLFWWindow(), framebuffer_size_callback);

    WindowState state{};
    state.lastX = static_cast<float>( window.getFramebufferWidth() ) / 2.0f;
    state.lastY = static_cast<float>( window.getFramebufferHeight() ) / 2.0f;
    glfwSetWindowUserPointer(window.getGLFWWindow(), & state);

    glfwSetKeyCallback(window.getGLFWWindow(), key_callback);
    glfwSetCursorPosCallback(window.getGLFWWindow(), mouse_callback);
    glfwSetScrollCallback(window.getGLFWWindow(), scroll_callback);


    core::Shader shader{ "assets/shaders/shader.vert", "assets/shaders/shader.frag" };

    const std::vector<float> vertices = {
        - 0.5f, - 0.5f, - 0.5f, 0.0f, 0.0f,
        0.5f, - 0.5f, - 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, - 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, - 0.5f, 1.0f, 1.0f,
        - 0.5f, 0.5f, - 0.5f, 0.0f, 1.0f,
        - 0.5f, - 0.5f, - 0.5f, 0.0f, 0.0f,

        - 0.5f, - 0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, - 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        - 0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        - 0.5f, - 0.5f, 0.5f, 0.0f, 0.0f,

        - 0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        - 0.5f, 0.5f, - 0.5f, 1.0f, 1.0f,
        - 0.5f, - 0.5f, - 0.5f, 0.0f, 1.0f,
        - 0.5f, - 0.5f, - 0.5f, 0.0f, 1.0f,
        - 0.5f, - 0.5f, 0.5f, 0.0f, 0.0f,
        - 0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, - 0.5f, 1.0f, 1.0f,
        0.5f, - 0.5f, - 0.5f, 0.0f, 1.0f,
        0.5f, - 0.5f, - 0.5f, 0.0f, 1.0f,
        0.5f, - 0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        - 0.5f, - 0.5f, - 0.5f, 0.0f, 1.0f,
        0.5f, - 0.5f, - 0.5f, 1.0f, 1.0f,
        0.5f, - 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, - 0.5f, 0.5f, 1.0f, 0.0f,
        - 0.5f, - 0.5f, 0.5f, 0.0f, 0.0f,
        - 0.5f, - 0.5f, - 0.5f, 0.0f, 1.0f,

        - 0.5f, 0.5f, - 0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, - 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        - 0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        - 0.5f, 0.5f, - 0.5f, 0.0f, 1.0f
    };

    unsigned int VAO;
    unsigned int VBO;
    const core::Texture tex = core::Texture("assets/textures/wall.jpg");

    glGenVertexArrays(1, & VAO);
    glGenBuffers(1, & VBO);
    glBindVertexArray(VAO);

    // VBO setup
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), ( void * )0);
    glEnableVertexAttribArray(0);

    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), ( void * )(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Sends texture data to frag shader using uniforms
    shader.setUniform<int>("tex", 0);
    shader.use();


    /* Defines our Camera class to automatically change our view and perspective
     * matrices to simulate a camera */
    core::Camera camera({ .Pos = glm::vec3(0.0f, 0.0f, 5.0f), .Speed = 7.5f, .MouseSens = 0.1f });
    state.pCamera = & camera;

    // For 3D rendering we need to enable the z buffer
    glEnable(GL_DEPTH_TEST);
    // Enables true transparency for images
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    core::FPSCounter fps;
    window.setClearColour(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

    while(!window.shouldClose())
    {
        window.updateTime();
        fps.update(window.getDeltaTime());
        processInput(window.getGLFWWindow(), camera, window.getDeltaTime());

        window.clear();
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

        auto model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, - 3.0f));
        model = glm::rotate(model, static_cast<float>( window.getWindowTime() ), glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setUniform<glm::mat4>("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
        window.endImgui();
        window.swapBuffers();
        window.pollEvents();
    }
    glDeleteVertexArrays(1, & VAO);
    glDeleteBuffers(1, & VBO);
    glfwTerminate();
    return 0;
}
