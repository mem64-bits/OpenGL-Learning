#include <glad/gl.h>
#include "include/Shader.h"
#include  <GLFW/glfw3.h>
#include <iostream>

//Global Variables are bad (quick and easy way to setup a toggle)
bool wireframe{ false };

// Foward declares functions to make function definition cleaner
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);

void checkShaderCompileStatus(unsigned int shader);
void checkShaderProgramStatus(unsigned int shaderProgram);

int main()
{
    // Initilizes glfw and OpenGL context versions
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // the next version to try if major version fails
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 800;
    int height = 600;
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Triangle", NULL, NULL);

    // GLFW Initilization check
    if (window == NULL)
    {
        std::cerr << "Failed to setup GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initilizes and checks glad 
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // Uses our new shader object cutting down on boilerplate code
    Shader shader{ "assets/shaders/shader.vert","assets/shaders/shader.frag" };

    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right (0)
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left  (1)
        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   // top right   (2)
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f    // top left    (3)
    };

    unsigned int indices[]{
        0,1,2,
        0,2,3
    };

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Enable Wireframe Rendering
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //Render Loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        if (wireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        //float timeValue = static_cast<float>(glfwGetTime());
        glClear(GL_COLOR_BUFFER_BIT);
        processInput(window);

        // uses shader program created in our class object
        shader.use();
        glBindVertexArray(VAO); 
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void key_callback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int key, [[maybe_unused]] int scancode, [[maybe_unused]] int action, [[maybe_unused]] int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        wireframe = !wireframe;
    }
        
}
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

