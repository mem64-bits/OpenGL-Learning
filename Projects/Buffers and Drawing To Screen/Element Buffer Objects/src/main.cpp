#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

// (See Line 67 for example)

// Forward Declarations to make code cleaner
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

// GLSL Shaders defined as raw strings to format multiline string easier
const char *vertexShaderSource = R"(
	#version 330 core
	layout (location = 0) in vec3 aPos;
	void main()
	{
		gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	}
)";

// GLSL Fragment Shader, controls colour of the triangle
const char *fragmentShaderSource = R"(
	#version 330 core
	out vec4 FragColor;
	void main()
	{
		FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	}
)";

int main() {
  /*---------------------------------------------------------- */
  // Initializes GLFW
  /*---------------------------------------------------------- */
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,
                 3); // the next version to try if major version fails
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Windows dimensions shared between glfw and OpenGL
  int width = 800;
  int height = 600;

  // Creates GLFW Window
  GLFWwindow *window = glfwCreateWindow(800, 600, "Using EBO", NULL, NULL);

  // GLFW Initialization Check
  if (window == NULL) {
    std::cerr << "Failed to setup GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // Initializes GLAD
  if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  glViewport(0, 0, width, height);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  /*--------------------------------------------------------------------------*/
  // Vertex Shader Setup
  /*--------------------------------------------------------------------------*/
  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);

  // turns shader into usable object
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader); // compiles shader for use

  // Error Handling Variables for Shaders
  int success;
  char infoLog[512];

  // Shader Compilation Check
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cerr << "Failed to compile vertex shader\n" << infoLog << std::endl;
  }

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); 
 
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cerr << "Failed to compile Fragment Shader\n" << infoLog << std::endl;
  }

  // After the shaders have been created and compiled they need to be linked to
  // create a program
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
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  glUseProgram(shaderProgram);

  // After the shaderProgram is made we can delete the shaders
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Vertex Data
  float vertices[] = {
      // First Triangle
      0.5f,  0.5f,  0.0f, // top right (vertex 0)
      0.5f,  -0.5f, 0.0f, // bottom right (vertex 1)
      -0.5f, -0.5f, 0.0f, // bottom left (vertex 2)
      -0.5f, 0.5f,  0.0f  // top left (vertex 3)
  };

  /*A Triangle is the simplest shape we can make in 3D graphics,
   * but how can we link up triangles to make a more complex shape
   * like a rectangle ?
   *
   * To do this we use Element Buffer Objects (EBO),
   * EBO's define each triangle by the three vertices
   * that they consist of and store them as an array
   */

  // indice data for EBO
  unsigned int indices[] = {
      0, 1, 3, // First Triangle
      1, 2, 3  // Second Triangle
  };

  /*Setting up the buffers*/
  unsigned int VBO;
  unsigned int VAO;
  // Defines variable for EBO
  unsigned EBO;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  // Creates buffer for EBO
  glGenBuffers(1, &EBO);

  // We bind the VAO first so that all attributes are saved into the VAO
  glBindVertexArray(VAO);

  /*---------------------------------------------------------------------*/
  // VBO Setup
  /*--------------------------------------------------------------------*/
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // Copy VBO data to bound VAO
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // The EBO is bound next
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Turn on wireframe mode
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Render Loop
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    processInput(window);

    /*Things we want to render start from here*/
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}
