#include <glad/glad.h>
#include  <GLFW/glfw3.h>
#include <iostream>
/*This program uses 2 separate VBO'S and VAO's to draw
 *triangles to the screen*/

 // Forward Declarations to make code cleaner
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

void checkShaderCompileStatus(unsigned int shader);

void checkShaderProgramStatus(unsigned int shaderProgram);

// GLSL Shaders defined as raw strings to simplify code
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main()
{
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)";

// GLSL Fragment Shader, controls colour of the triangle
const char* fragmentShaderSource1 = R"(
#version 330 core
out vec4 FragColor;
void main()
{
   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)";

const char* fragmentShaderSource2 = R"(
#version 330 core
out vec4 FragColor;
void main()
{
   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
)";

int main()
{
    /*---------------------------------------------------------- */
    // Initializes GLFW
    /*---------------------------------------------------------- */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // the next version to try if major version fails
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // Windows dimensions shared between glfw and OpenGL
    int width = 800;
    int height = 600;

    // Creates GLFW Window
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Triangle", NULL, NULL);

    // GLFW Initialization Check
    if (window == NULL)
    {
        std::cerr << "Failed to setup GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initializes GLAD 
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    /*--------------------------------------------------------------------------*/
    // Shader Setup
    /*--------------------------------------------------------------------------*/
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    
    // turns shader into usable object
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader); // compiles shader for use

    // Shader Compilation Check
    checkShaderCompileStatus(vertexShader);

    unsigned int fragmentShader1;
    unsigned int fragmentShader2;
    fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
    fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader1, 1, &fragmentShaderSource1, NULL);
    glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);

    glCompileShader(fragmentShader1);
    glCompileShader(fragmentShader2);

    checkShaderCompileStatus(fragmentShader1);
    checkShaderCompileStatus(fragmentShader2);

    // After the shaders have been created and compiled they need to be linked to create a program
    unsigned int shaderProgram1;
    unsigned int shaderProgram2; 

    shaderProgram1 = glCreateProgram();
    shaderProgram2 = glCreateProgram();

    // Links shaders to create shader program
    glAttachShader(shaderProgram1, vertexShader);
    glAttachShader(shaderProgram1, fragmentShader1);
    glLinkProgram(shaderProgram1);
    checkShaderProgramStatus(shaderProgram1);

    glAttachShader(shaderProgram2, vertexShader);
    glAttachShader(shaderProgram2, fragmentShader2);
    glLinkProgram(shaderProgram2);
    checkShaderProgramStatus(shaderProgram2);

    // After the shaderProgram is made we can delete the shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader1);
    glDeleteShader(fragmentShader2);

    /*---------------------------------------------------------------------------*/
    // Vertex Data and Buffer Setup (VBO and VAO)
    /*---------------------------------------------------------------------------*/

    // Vertex Data (Forms Triangle)
   const float triangles[2][9] = {
         {
            -0.5f,  0.5f, 0.0f, // Top (0)
            -1.0f, -0.5f, 0.0f, // Left (2)
             0.0f, -0.5f, 0.0f // Right (2)
         },

        {
            0.5f,  0.5f, 0.0f, // Top (0)
            0.0f, -0.5f, 0.0f, // Left (2)
            1.0f, -0.5f, 0.0f // Right (2)
        }
    };

    unsigned int VBOs[2];
    unsigned int VAOs[2];


    for (std::size_t i = 0; i < 2; i++)
    {
        glGenBuffers(1, &VBOs[i]);
        glGenVertexArrays(1, &VAOs[i]);
        glBindVertexArray(VAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangles[i]), triangles[i], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }


    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        // Clears screen and draws blue background
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        processInput(window);

        /*Things we want to render start from here*/
        glUseProgram(shaderProgram1);
        glBindVertexArray(VAOs[0]); // Rebind the VAO with all our configuration for drawing
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);
        glUseProgram(shaderProgram2);
        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void checkShaderCompileStatus(unsigned int shader)
{
    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        int shaderType;
        std::string shaderName{ "Unknown Shader" };
        glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);

        switch (shaderType)
        {
        case GL_VERTEX_SHADER:
            shaderName = "Vertex Shader";
            break;

        case GL_FRAGMENT_SHADER:
            shaderName = "Fragment Shader";
            break;
        default:
            return;
        }
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
    }
}

void checkShaderProgramStatus(unsigned int shaderProgram)
{
    int  success;
    char infoLog[512];
    // after the program is linked we do a check to see if its valid
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Shader Program failed to link!: \n" << infoLog << std::endl;
    }
}
