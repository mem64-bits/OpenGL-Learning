#include <glad/gl.h>
#include  <GLFW/glfw3.h>
#include <iostream>

// Forward Declarations to make code cleaner
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

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
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
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
         0.0f, -0.5f, 0.0f,  //  1
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


    // Hacky way to scale down matrix
    for (int i = 0; i < std::size(vertices); i++)
    {
        vertices[i] *= 0.6f;
    }

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,GL_STATIC_DRAW);
    
    // Sets up EBO Buffer and copies indice data to it
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);



    /*--------------------------------------------------------------------------*/
    // Vertex Shader Setup
    /*--------------------------------------------------------------------------*/

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // turns shader into usable object
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader); // compiles shader for use

    // Error Handling Variables
    int  success;
    char infoLog[512];

    // Shader Compilation Check
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Vertex Shader Failed to compile!: \n" << infoLog << std::endl;
    }
    /*---------------------------------------------------------------------------------------*/
    // Fragment Shader Setup
    /*--------------------------------------------------------------------------------------*/
	unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource1, NULL);
    glCompileShader(fragmentShader);

    // Checks the status of the shader to detect errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Fragment Shader Failed to compile!: \n" << infoLog << std::endl;
    }

    // After the shaders have been created and compiled they need to be linked to create a program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // Links shaders to create shader program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // after the program is linked we do a check to see if its valid
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Shader Program failed to link:\n" << infoLog << std::endl;
    }

    glUseProgram(shaderProgram);

    // After the shaderProgram is made we can delete the shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //Toggle Wireframe rendering mode
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  
    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    	processInput(window);

        /*Things we want to render start from here*/
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
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



