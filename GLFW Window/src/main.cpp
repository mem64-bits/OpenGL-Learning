#include <glad/glad.h>
#include  <GLFW/glfw3.h>
#include <iostream>

// Forward Declarations to make code cleaner
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);


int main()
{
    /*---------------------------------------------------------- */
    // Initializing GLFW
    /*---------------------------------------------------------- */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Tells GLFW what version of OpenGL to use
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // the next version to try if major version fails
    // Tells OpenGL to use modern profile with no extensions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 


    // Windows dimensions shared between glfw and OpenGL
    int width = 800;
    int height = 600;

    // Creates GLFW Window
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Triangle", NULL, NULL);

    // GLFW Initialization Check, cleanly exits program if error occurs 
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
    // ensures viewport and window are the same size by using defined variables
    glViewport(0, 0, width, height); 

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // detects changes in the windows size


    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        // Clears the screen to replace with custom colour
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Gets Input
    	processInput(window); 
        //(2x Buffering) Swaps the front and back buffers, presenting the new frame and preventing screen tearing.
        glfwSwapBuffers(window);
        // Checks if any important events have happened, and calls corresponding functions
        glfwPollEvents(); 
    }
    glfwTerminate(); // Frees memory that was allocated when window is closed
    return 0;

}

// Function that OpenGL calls to resize viewport whenever window size is changed
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//Sets up method where input can be checked for
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

