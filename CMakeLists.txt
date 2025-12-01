cmake_minimum_required(VERSION 3.25)
project(OpenGL-Learning C CXX)

find_package(Python3 COMPONENTS Interpreter REQUIRED)

# Check for Jinja2 (Common source of errors)
execute_process(
    COMMAND ${Python3_EXECUTABLE} -c "import jinja2"
    RESULT_VARIABLE JINJA_STATUS
    ERROR_QUIET
)
if(NOT JINJA_STATUS EQUAL 0)
    message(FATAL_ERROR "Python 'jinja2' not found. Run: pip install jinja2")
endif()


set(GLAD_SOURCES_DIR "${PROJECT_SOURCE_DIR}/external/glad/")
add_subdirectory("${GLAD_SOURCES_DIR}/cmake" glad_cmake)
glad_add_library(glad_gl_core_46 REPRODUCIBLE API gl:core=4.6)


include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.4 # Use specific tags for stability (vs master)
)
# Disable GLFW bloat (docs, tests) to speed up build
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# Use the OS default OpenGL header (we will replace it with GLAD later)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE) 

FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        1.0.2 # Latest stable tag
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

# Create a library target for STB so we can link it easily
# STB is "header only", so INTERFACE is appropriate.
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})


# target_link_libraries(MyApp
#     PRIVATE
#         glad_gl_core_46      # The auto-generated GLAD target
#         glfw        # Windowing
#         glm::glm    # Math
#         stb         # Images
# )