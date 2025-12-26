cmake_minimum_required(VERSION 3.25)
project(OpenGL-Learning CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fix for Windows headers breaking std::min/max
if (MSVC)
    add_compile_definitions(NOMINMAX)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif ()


#  USER OPTIONS

option(USE_SHARED_LIBS "Build external libraries as Shared (Dynamic) instead of Static" OFF)
option(BUILD_ALL_LESSONS "Build all discovered projects by default" ON)

# ==============================================================================
#  PYTHON VENV & GLAD GENERATION
# ==============================================================================

# Creates python venv environment in cmake build folder to generate glad library
find_package(Python3 COMPONENTS Interpreter REQUIRED)
set(VENV_DIR "${CMAKE_BINARY_DIR}/venv")

# Windows and Linux Support
if (WIN32)
    set(VENV_PYTHON_EXE "${VENV_DIR}/Scripts/python.exe")
else ()
    set(VENV_PYTHON_EXE "${VENV_DIR}/bin/python3")
endif ()

if (NOT EXISTS "${VENV_PYTHON_EXE}")
    message(STATUS "Creating Python Virtual Environment...")
    execute_process(COMMAND "${Python3_EXECUTABLE}" -m venv "${VENV_DIR}")
    execute_process(COMMAND "${VENV_PYTHON_EXE}" -m pip install --upgrade pip glad2 jinja2 --quiet)
endif ()

set(GLAD_OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glad")
set(GLAD_HEADER "${GLAD_OUT_DIR}/include/glad/gl.h")

if (NOT EXISTS "${GLAD_HEADER}")
    message(STATUS "Generating OpenGL 4.6 Core files (GLAD 2)...")
    execute_process(COMMAND "${VENV_PYTHON_EXE}" -m glad
            --api gl:core=4.6
            --out-path "${GLAD_OUT_DIR}"
            --reproducible c)
endif ()


# External Library Setup

set(GLAD_TYPE STATIC)
if (USE_SHARED_LIBS)
    set(GLAD_TYPE SHARED)
endif ()

# Copies glad files to output folder
add_library(glad ${GLAD_TYPE} "${GLAD_OUT_DIR}/src/gl.c")
target_include_directories(glad SYSTEM PUBLIC "${GLAD_OUT_DIR}/include")

# GLFW Setup
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# GLM & STB Interface Targets (Header Only)
add_library(glm INTERFACE)
target_include_directories(glm SYSTEM INTERFACE "${CMAKE_SOURCE_DIR}/external/glm")

add_library(stb INTERFACE)
target_include_directories(stb SYSTEM INTERFACE "${CMAKE_SOURCE_DIR}/external/stb")

# IMGUI Headers & Implementation Target
add_library(imgui_headers INTERFACE)
target_include_directories(imgui_headers SYSTEM INTERFACE
        "${CMAKE_SOURCE_DIR}/external/imgui"
        "${CMAKE_SOURCE_DIR}/external/imgui/backends"
)

# Gathers backend and implementation files for imgui
add_library(imgui_backends STATIC
        "${CMAKE_SOURCE_DIR}/external/imgui/imgui.cpp"
        "${CMAKE_SOURCE_DIR}/external/imgui/imgui_draw.cpp"
        "${CMAKE_SOURCE_DIR}/external/imgui/imgui_widgets.cpp"
        "${CMAKE_SOURCE_DIR}/external/imgui/imgui_tables.cpp"
        "${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_glfw.cpp"
        "${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3.cpp"
)
#Statically Compiles Imgui to one target making it easy to build with
target_link_libraries(imgui_backends PUBLIC imgui_headers PRIVATE glfw glad)

# Makes stb_define implementation a static library to avoid manually adding definition to every project
add_library(stb_impl STATIC "${CMAKE_SOURCE_DIR}/core/src/stb_define.cpp")
target_link_libraries(stb_impl PRIVATE stb)


# OpenGL Engine Setup (arranges our helper class code into the beginnings of a
# small game engine as more opengl concepts are learned)

set(CORE_SOURCES
        "${CMAKE_SOURCE_DIR}/core/src/Window.cpp"
        "${CMAKE_SOURCE_DIR}/core/src/Camera.cpp"
        "${CMAKE_SOURCE_DIR}/core/src/FPSCounter.cpp"
        "${CMAKE_SOURCE_DIR}/core/src/Shader.cpp"
        "${CMAKE_SOURCE_DIR}/core/src/Texture.cpp"
        "${CMAKE_SOURCE_DIR}/core/src/ImageLoader.cpp"
)

add_library(core STATIC ${CORE_SOURCES})

# Makes engine headers declarable with < > .eg #include <Shader.h>
target_include_directories(core SYSTEM PUBLIC "${CMAKE_SOURCE_DIR}/core/src")

# Link the core engine with required external libs
target_link_libraries(core PUBLIC
        glad
        glfw
        glm
        stb
        stb_impl        # Provides stbi_load symbols to the engine
        imgui_backends  # Provides ImGui symbols to the engine
)


# MODERN VS LEGACY
# (Makes new "Modern" engine setup compatible with older (LEGACY) project examples)


# Links to engine
add_library(common_libs INTERFACE)
target_link_libraries(common_libs INTERFACE core)

add_library(common_libs_old INTERFACE)
target_link_libraries(common_libs_old INTERFACE glad glfw glm stb stb_impl)

# HELPER FUNCTIONS
function(silence_target target)
    if (TARGET ${target})
        if (MSVC)
            target_compile_options(${target} PRIVATE /W0 /WX-)
        else ()
            target_compile_options(${target} PRIVATE -w -Wno-error)
        endif ()
    endif ()
endfunction()

# Stops massive warnings being generated from external libaries
silence_target(glad)
silence_target(glfw)
silence_target(imgui_backends)
silence_target(stb_impl)
silence_target(core)

function(create_lesson TARGET_NAME)
    # optional LEGACY parameter to build non engine projects
    set(options LEGACY)
    cmake_parse_arguments(ARG "${options}" "" "" ${ARGN})

    if (BUILD_ALL_LESSONS)
        add_executable(${TARGET_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    else ()
        add_executable(${TARGET_NAME} EXCLUDE_FROM_ALL "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    endif ()

    if (ARG_LEGACY)
        message(STATUS "Configuring ${TARGET_NAME} as LEGACY (Non-Engine)")
        target_link_libraries(${TARGET_NAME} PRIVATE common_libs_old)
        target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    else ()
        message(STATUS "Configuring ${TARGET_NAME} as MODERN (Engine Mode)")
        target_link_libraries(${TARGET_NAME} PRIVATE common_libs)
    endif ()

    if (MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE /W4 /external:W3 /permissive- /GS /MP
                $<$<CONFIG:Release>:/O2 /Oi /Gy /arch:AVX2>)
        target_compile_definitions(${TARGET_NAME} PRIVATE $<$<CONFIG:Release>:NDEBUG>)
    endif ()

    # Copies assets to build folder if changed
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
        file(GLOB_RECURSE ASSET_FILES "${CMAKE_CURRENT_SOURCE_DIR}/assets/*")
        set(COPIED_ASSETS "")

        foreach (ASSET_SOURCE_PATH ${ASSET_FILES})
            file(RELATIVE_PATH ASSET_REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${ASSET_SOURCE_PATH}")
            set(ASSET_DEST_PATH "${CMAKE_CURRENT_BINARY_DIR}/${ASSET_REL_PATH}")
            add_custom_command(OUTPUT ${ASSET_DEST_PATH}
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASSET_SOURCE_PATH} ${ASSET_DEST_PATH}
                    DEPENDS ${ASSET_SOURCE_PATH})
            list(APPEND COPIED_ASSETS ${ASSET_DEST_PATH})
        endforeach ()

        add_custom_target(copy_assets_${TARGET_NAME} ALL DEPENDS ${COPIED_ASSETS})
        add_dependencies(${TARGET_NAME} copy_assets_${TARGET_NAME})
    endif ()
    set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endfunction()

add_subdirectory(Projects)