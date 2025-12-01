#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GLFWCallbacks.h"
#include "Shader.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h> // For loading image files


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // the next version to try if major version fails
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int winWidth = 800;
    int winHeight = 600;
    GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL Textures", NULL, NULL);

    if (window == NULL)
    {
        std::cerr << "Failed to setup GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
   
    glViewport(0, 0, winWidth, winHeight);

	// Struct made to handle states of window easier e.g wireframe mode
    WindowState* state{};
    /* sets the pointer to the window so that the window pointer
     * can be accessed in any scope*/
    glfwSetWindowUserPointer(window, &state); 

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  
    Shader shader{"Shaders/vertShader.vert", "Shaders/fragShader.frag"};

    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    
    unsigned int indices[]{
        0, 1, 3,
        1, 2, 3

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

    // Sets vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Sets colour attribute pointers
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Sets texture attribute pointers
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    /*-----------------------------------*
    * Using Image Textures in OpenGL    *
    *-----------------------------------*/

    // Image attribute variables that stb fill automatically
    int imgWidth;
    int imgHeight;
    int nrChannels;

    // loads image and fills attribute variables with data automatically
    unsigned char* data = stbi_load("Textures/wall.jpg", &imgWidth, &imgHeight, &nrChannels, 0);

    // Generates storage for the image texture
    unsigned int texture;
    glGenTextures(1, &texture); // makes OpenGL texture to store image data in
    glBindTexture(GL_TEXTURE_2D, texture); // tells OpenGL what texture to use

    // sets texture wrapping & filtering options on bound texture object
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // Only creates OpenGL Texture and mipmap if image data from stbi_load is valid
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data); // deletes image in memory once we don't need it saving space

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        processInput(window);
        shader.use();

        /*
         * To use the texture that we have created we must bind the texture to automatically
         * sample it, and use it in our program
         */
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

