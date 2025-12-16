#include <glad/gl.h>
#include  <GLFW/glfw3.h>
#include <iostream>

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
const char* fragmentShaderSource = R"(// Fragment Shader
#version 330 core
out vec4 FragColor;
void main()
{
   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
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
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
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


    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkShaderCompileStatus(fragmentShader);

    // After the shaders have been created and compiled they need to be linked to create a program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // Links shaders to create shader program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkShaderProgramStatus(shaderProgram);

    // After the shaderProgram is made we can delete the shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /*---------------------------------------------------------------------------*/
    // Vertex Data and Buffer Setup (VBO and VAO)
    /*---------------------------------------------------------------------------*/

    // Vertex Data (Forms Triangle)
    float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
    };

    // Indice data for EBO (Useful for complex shapes)
    unsigned int indices[]{
        0,1,2
    };

    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

  
    // States type of information stored in buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Copies vertex data to the vertex buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // EBO Setup
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind the VBO as it's now registered in the VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind the VAO
    glBindVertexArray(0);


    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        // Clears screen and draws blue background
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        processInput(window);

        /*Things we want to render start from here*/
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // Rebind the VAO with all our configuration for drawing
        glDrawElements(GL_TRIANGLES, 3,GL_UNSIGNED_INT,0);

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
