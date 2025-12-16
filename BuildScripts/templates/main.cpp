#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "glfwCallBacks.h"
#include "Shader.h"

int main() {
    // 1. Init GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 2. Window Config
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    constexpr int width = 800;
    constexpr int height = 600;
    // Placeholder for python script dynamically replace with configured project name
    GLFWwindow* window = glfwCreateWindow(width, height, "{{PROJECT_NAME}}", nullptr, nullptr);

    if (!window) {
        std::cerr << "Failed to create window" <<'\n';
        glfwTerminate(); // Fixed: Terminate BEFORE returning
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 3. Init GLAD
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << '\n';
        return -1;
    }

    // Viewport is important!
    glViewport(0, 0, width, height);

    unsigned int VBO, VAO, EBO;

    // FIXED: Changed Y coords to make an actual triangle shape
    constexpr float vertices[]{
        // positions         // colors
         0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // Top
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // Bottom Left
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, // Bottom Right
    };

    constexpr unsigned int indices[]{
        0, 1, 2
    };

    Shader shader {"assets/shaders/shader.vert", "assets/shaders/shader.frag"};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // FIXED: Stride is now 6 * float (3 pos + 3 color)
    int stride = 6 * sizeof(float);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *) 0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 4. Main Loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        processInput(window);

        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}