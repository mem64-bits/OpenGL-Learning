#include <glad/gl.h>
#include  <GLFW/glfw3.h>
#include "include/GLFWCallbacks.h"
#include "include/Shader.h"
#include "include/Texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>



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
    GLFWwindow *window = glfwCreateWindow(winWidth, winHeight, "OpenGL Transformations", nullptr, nullptr);

    if (window == nullptr) {
        std::cerr << "Failed to setup GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGL((GLADloadfunc) glfwGetProcAddress)) {
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

    // Correct vertex data for a full-screen quad
    float vertices[] = {
        // positions          // colors          // texture coords
        1.0f,    1.0f,  0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // Top Right
         1.0f,  -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom Right
        -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Bottom Left
        -1.0f,   1.0f,  0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f // Top Left
    };

    // Correct indices to draw the two triangles for the quad
    unsigned int indices[] = {
        0, 1, 3, // First Triangle: Top-Right, Bottom-Right, Top-Left
        1, 2, 3 // Second Triangle: Bottom-Right, Bottom-Left, Top-Left
    };

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Texture tex1 = Texture("assets/textures/KazSmile.jpg");
    Texture tex2 = Texture("assets/textures/fire.png");

    // Creates Buffers needed for VAO, VBO, EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Saves buffer config to VAO
    glBindVertexArray(VAO);

    // VBO setup
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Sets up EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    
    shader.use();
    // Sets uniforms for textures in shader
    shader.setUniform<int>("texture1", 0);
    shader.setUniform<int>("texture2", 1);


    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        processInput(window);
         
        shader.use();  
        tex1.bindTexture(0);
        tex2.bindTexture(1);

        //Creates identity matrix to perform transformations on
        glm::mat4 trans = glm::mat4(1.0f);

        /*In 3D Graphics we want to follow the rule T.R.S when applying transformations
        on translation matrices
        
        1. Translation
        2. Rotation
        3. Scaling

        This ensures that the math done on the object is appiled in the correct order
        which is actually S.R.T
        */

        /* The translation matrix can be used to "translate" or move an object 
         * in the direction of a given vector */
        trans = glm::translate(trans, glm::vec3(-0.5f, 0.0f, 0.0f));
        trans = glm::rotate(trans, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 0.0f, -1.0f));
       
        /* We can also scale our object over time using frame-time that is
		normalised from a range of [-1, 1] by sin()*/
        float scaleValue = static_cast<float>(sin(glfwGetTime()));
        trans = glm::scale(trans, glm::vec3(scaleValue, scaleValue, scaleValue));
      
        // Sets transform uniform in vertex shader
        shader.setUniform<glm::mat4>("transform", trans);
        // Updates shader states in fragment shader
        shader.setUniform<int>("shaderState", state.shaderState);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}

