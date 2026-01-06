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

/*See glsl files for diffuse lighting math*/

int main()
{
    core::Window window({ .name = "DiffuseLighting", .vSync = false });
    glfwSetInputMode(window.getGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window.getGLFWWindow(), framebuffer_size_callback);

    WindowState state{};
    state.lastX = static_cast<float>( window.getFramebufferWidth() ) / 2.0f;
    state.lastY = static_cast<float>( window.getFramebufferHeight() ) / 2.0f;
    glfwSetWindowUserPointer(window.getGLFWWindow(), &state);

    glfwSetKeyCallback(window.getGLFWWindow(), key_callback);
    glfwSetCursorPosCallback(window.getGLFWWindow(), mouse_callback);
    glfwSetScrollCallback(window.getGLFWWindow(), scroll_callback);

    // Sets up shaders for the cube and light source
    const core::Shader cubeShader{ "assets/shaders/cubeShader.vert", "assets/shaders/cubeShader.frag" };
    const core::Shader lightingShader{ "assets/shaders/lightShader.vert", "assets/shaders/lightShader.frag" };

    const std::vector<float> vertices = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
    };

    unsigned int VBO;
    unsigned int cubeVAO;
    unsigned int lightVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO);

    // VBO setup
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(vertices[0])),
                 vertices.data(),
                 GL_STATIC_DRAW);

    // Set up for cubeVAO
    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // normal vector attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), ( void * )(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Sets up light VAO
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);


    /* Defines our Camera class to automatically change our view and perspective
     * matrices to simulate a camera */
    core::Camera camera({ .Pos = glm::vec3(0.0f, 0.0f, 6.0f), .Speed = 7.5f, .MouseSens = 0.1f });
    state.pCamera = &camera;

    // For 3D rendering we need to enable the z buffer
    glEnable(GL_DEPTH_TEST);
    // Enables true transparency for images
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    core::FPSCounter fps;
    window.setClearColour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);


    while(!window.shouldClose())
    {
        window.updateTime();
        fps.update(window.getDeltaTime());
        processInput(window.getGLFWWindow(), camera, window.getDeltaTime());

        window.clear();
        window.beginImgui();
        fps.drawUI();

        cubeShader.use();
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(window.getFramebufferWidth(), window.getFramebufferHeight());
        cubeShader.setUniform<glm::mat4>("view", view);
        cubeShader.setUniform<glm::mat4>("projection", projection);
        cubeShader.setUniform<glm::vec3>("objectColour", glm::vec3(1.0f, 0.5f, 0.31f));
        cubeShader.setUniform<glm::vec3>("lightColour", glm::vec3(1.0f, 1.0f, 1.0f));
        cubeShader.setUniform<glm::vec3>("lightPos", lightPos);

        auto model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, static_cast<float>( window.getWindowTime() ), glm::vec3(0.0f, 1.0f, 0.0f));
        cubeShader.setUniform<glm::mat4>("model", model);

        glBindVertexArray(cubeVAO);// Use the CUBE VAO
        glDrawArrays(GL_TRIANGLES, 0, 36);


        lightingShader.use();
        lightingShader.setUniform<glm::mat4>("view", view);
        lightingShader.setUniform<glm::mat4>("projection", projection);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.125f));// Make the lamp smaller
        lightingShader.setUniform<glm::mat4>("model", model);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        window.endImgui();
        window.swapBuffers();
        window.pollEvents();
    }
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
