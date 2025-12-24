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

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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
    GLFWwindow *window = glfwCreateWindow(winWidth, winHeight, "Cameras And Viewing", nullptr, nullptr);
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

    // 1. Initialize ImGui Context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();

    // 2. Initialize Backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Get the actual framebuffer size (accounts for DPI scaling)
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    WindowState state{};
    state.lastX = static_cast<float>(winWidth) / 2.0f;
    state.lastY = static_cast<float>(winHeight) / 2.0f;
    glfwSetWindowUserPointer(window, &state);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

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
    shader.setUniform<int>("texture", 0);
    shader.use();


    /* Defines our Camera class to automatically change our view and perspective
     * matrices to simulate a camera */
    Camera camera({.Pos = glm::vec3(0.0f, 0.0f, 5.0f), .Speed = 4.5f, .MouseSens = 0.06f});
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
    float lastFrame = 0.0f;
    float deltaTime = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        // gets the window timing variables needed for calculating deltaTime
        const auto timeValue = static_cast<float>(glfwGetTime());
        float currentFrame = timeValue;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window, camera, deltaTime);

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Build the FPS Counter Window
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        ImGui::Begin("Performance", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "FPS: %.1f", 1.0f / deltaTime); // Yellow text
        ImGui::End();

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        tex.bindTexture(0);
        shader.setUniform<int>("shaderState", state.shaderState);
        glBindVertexArray(VAO);

        glm::mat4 view = camera.getViewMatrix();
        shader.setUniform<glm::mat4>("view", view);

        const glm::mat4 projection = camera.getProjectionMatrix(winWidth, winHeight);
        shader.setUniform<glm::mat4>("projection", projection);

        for (unsigned int i = 0; i < std::size(cubePositions); i++)
        {
            /* We need to update the model matrix for each cube every frame
             * as transformations need to be updated every frame*/
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);

            if (i % 2 == 0)
            {
                model = glm::rotate(model, timeValue, glm::vec3(1.0f, 1.0f, 1.0f));
            }
            if (i % 2 != 0)
            {
                model = glm::rotate(model, timeValue, glm::vec3(0.0f, -1.0f, 0.0f));
            }

            const float rotateAngle = 20.0f * static_cast<float>(i);
            model = glm::rotate(model, rotateAngle, glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setUniform<glm::mat4>("model", model);

            // A cube has 36 vertices  (6 faces * 2 triangles * 3 vertices each)
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

