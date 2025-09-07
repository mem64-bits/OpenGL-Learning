#include <glad/glad.h>
#include  <GLFW/glfw3.h>
#include <iostream>
#include <vector>

// Forward Declarations to make code cleaner
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

void checkShaderCompileStatus(unsigned int shader);

void checkShaderProgramStatus(unsigned int shaderProgram);

// GLSL Shaders defined as Raw String to make multiline string formatting easier
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main()
{
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)";

// GLSL Fragment Shader, colours the triforce yellow
const char* fragmentShaderSource1 = R"(

#version 330 core
out vec4 FragColor;
void main()
{
FragColor = vec4(1.0, 0.843, 0.0, 1.0);
}

)"; 

const char* fragmentShaderSource2 = R"(

#version 330 core
out vec4 FragColor;
void main()
{
	FragColor = vec4(1.0,0.0,0.0,0.0);
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
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL TriForce", NULL, NULL);

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


    // Vertex Data (Simplified shared vertex points by using the EBO to connect them)
    float vertices[] = {
        // first triangle
        -1.0f, -0.5f, 0.0f,   // 0
        -0.0f, -0.5f, 0.0f,  //  1
        -0.5f, 0.5f, 0.0f,  //   2

        // second triangle
         0.0f, -0.5f, 0.0f,    // 3
         1.0f, -0.5f, 0.0f,   //  4
         0.5f, 0.5f, 0.0f,   //   5

         0.0f, 1.5f, 0.0f    // top 6
    };

    // thanks to the ebo we can connect any 3 vertices we to create triangles
    unsigned int indices[] = {
        1, 0, 2, // Left Triangle
        3, 4, 5, // Right Triangle
        2, 5, 6 // Top Triangle
    };

    // All of these triangles combine to form the triforce


    /*Setting Up VBO,VAO and EBO Buffers*/
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;

    // Creates Buffers for VBO and VAO
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);


    // Binds VAO to save buffer attributes to
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Sets the data type held in the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Sets up EBO Buffer and copies indice data to it
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);



    /*--------------------------------------------------------------------------*/
    // Shader Setup
    /*--------------------------------------------------------------------------*/
     
    // defines both shaders
    unsigned int vertexShader;
    unsigned int fragmentShader1;
    unsigned int fragmentShader2;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
    fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glShaderSource(fragmentShader1, 1, &fragmentShaderSource1, NULL);
    glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);

	glCompileShader(vertexShader); 
    glCompileShader(fragmentShader1);
    glCompileShader(fragmentShader2);

    checkShaderCompileStatus(vertexShader);
    checkShaderCompileStatus(fragmentShader1);
    checkShaderCompileStatus(fragmentShader2);

    unsigned int shaderProgram1;
    unsigned int shaderProgram2;

    shaderProgram1 = glCreateProgram();
    shaderProgram2 = glCreateProgram();

    glAttachShader(shaderProgram1, vertexShader);
    glAttachShader(shaderProgram1, fragmentShader1);
    glLinkProgram(shaderProgram1);
    checkShaderProgramStatus(shaderProgram1);


    glAttachShader(shaderProgram2, vertexShader);
    glAttachShader(shaderProgram2, fragmentShader2);
    glLinkProgram(shaderProgram2);
    checkShaderProgramStatus(shaderProgram2);

    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        processInput(window);

        /*Things we want to render start from here*/
        
        glBindVertexArray(VAO);
        glUseProgram(shaderProgram1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram1);
    glDeleteProgram(shaderProgram2);
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


