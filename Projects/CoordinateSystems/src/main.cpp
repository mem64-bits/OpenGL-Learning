#include <glad/gl.h>
#include  <GLFW/glfw3.h>
#include "GLFWCallbacks.h"
#include "Shader.h"
#include "Texture.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // the next version to try if major version fails
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Fixes scaling problems on wayland linux
#ifdef __linux__
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
#endif

    constexpr int winWidth = 800;
    constexpr int winHeight = 600;
    GLFWwindow *window = glfwCreateWindow(winWidth, winHeight, "Coordinate Systems", nullptr, nullptr);

    if (window == nullptr)
    {
        std::cerr << "Failed to setup GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGL((GLADloadfunc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Get the actual framebuffer size (accounts for DPI scaling)
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    WindowState state{};
    glfwSetWindowUserPointer(window, &state);

    glfwSetKeyCallback(window, key_callback);

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
    const Texture tex = Texture("assets/textures/wall.jpg");

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

    shader.use();
    // Sends texture data to frag shader using uniforms
    shader.setUniform<int>("texture", 0);

    /* --------------------------------------------------------------------------
     * Setting up 3D viewing space using glm
     * --------------------------------------------------------------------------
     *
     * The 1st parameter defines the FOV (Field of view), which sets how large
     * the view space of the camera is.
     *
     * The second parameter sets the aspect ratio which is calculated by dividing
     * the viewport's width by its height.
     *
     * The third and fourth parameter set the near and far plane of the frustum.
     * We usually set the near distance to 0.1 and the far distance to 100.0.
     *
     * All the vertices between the near and far plane and inside the frustum
     * will be rendered.
     *
     *Whenever the near value of your perspective matrix is set too high (like 10.0),
     *OpenGL will clip all coordinates close to the camera (between 0.0 and 10.0),
     *This gives the effect of objects being clipped by the camera when getting to
     *close to them like in many games.
     */

    // The projection matrix defines the frustum of viewing into the scene
    const glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), static_cast<float>(winWidth) /
                             static_cast<float>(winHeight), 0.1f, 100.0f);

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


    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processInput(window);

        shader.use();
        tex.bindTexture(0);
        shader.setUniform<int>("shaderState", state.shaderState);
        glBindVertexArray(VAO);


        // The view matrix defines the position the camera is within the 3D world context
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        // Sets matrices needed for camera position and camera view
        shader.setUniform<glm::mat4>("projection", projection);
        shader.setUniform<glm::mat4>("view", view);
        const float timeValue = static_cast<float>(glfwGetTime());

        for (unsigned int i = 0; i < std::size(cubePositions); i++)
        {
            /* We need to update the model matrix for each cube every frame
             * as transformations need to be updated every frame*/
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);

            if (i % 2 == 0)
            {
                model = glm::rotate(model, timeValue, glm::vec3(1.0f, 1.0f, 1.0f));
            }
            if (i % 2 != 0)
            {
                model = glm::rotate(model, timeValue, glm::vec3(0.0f, -1.0f, 0.0f));
            }
            if (i % 3 == 0)
            {
                model = glm::rotate(model, timeValue, glm::vec3(-1.0f, 1.0f, -1.0f));
            }

            const float rotateAngle = 20.0f * i;
            model = glm::rotate(model, rotateAngle, glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setUniform<glm::mat4>("model", model);

            // A cube has 36 vertices  (6 faces * 2 triangles * 3 vertices each)
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

