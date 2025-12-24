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

# ==============================================================================
#  USER OPTIONS
# ==============================================================================
option(USE_SHARED_LIBS "Build external libraries as Shared (Dynamic) instead of Static" OFF)
option(BUILD_ALL_LESSONS "Build all discovered projects by default" ON)

# ==============================================================================
# 1. SETUP LOCAL PYTHON VENV (In Build Folder)
# ==============================================================================
find_package(Python3 COMPONENTS Interpreter REQUIRED)

# FIXED: Removed space in CMAKE_BINARY_DIR variable name
set(VENV_DIR "${CMAKE_BINARY_DIR}/venv")

if (WIN32)
    set(VENV_PYTHON_EXE "${VENV_DIR}/Scripts/python.exe")
else ()
    set(VENV_PYTHON_EXE "${VENV_DIR}/bin/python3")
endif ()

if (NOT EXISTS "${VENV_PYTHON_EXE}")
    message(STATUS "Creating Python Virtual Environment in: ${VENV_DIR}...")
    execute_process(
            COMMAND "${Python3_EXECUTABLE}" -m venv "${VENV_DIR}"
            RESULT_VARIABLE VENV_RESULT
    )
    if (NOT VENV_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to create virtual environment.")
    endif ()
    execute_process(COMMAND "${VENV_PYTHON_EXE}" -m pip install --upgrade pip --quiet)
endif ()

# ==============================================================================
# 2. INSTALL GLAD2 & GENERATE SOURCES
# ==============================================================================
set(GLAD_OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glad")
set(GLAD_HEADER "${GLAD_OUT_DIR}/include/glad/gl.h")

execute_process(COMMAND "${VENV_PYTHON_EXE}" -c "import glad2" RESULT_VARIABLE GLAD_PKG_EXISTS ERROR_QUIET)

if (NOT GLAD_PKG_EXISTS EQUAL 0)
    message(STATUS "Installing GLAD 2 package...")
    execute_process(
            COMMAND "${VENV_PYTHON_EXE}" -m pip install glad2 jinja2 --disable-pip-version-check --quiet
            RESULT_VARIABLE PIP_RESULT
    )
endif ()

if (NOT EXISTS "${GLAD_HEADER}")
    message(STATUS "Generating OpenGL 4.6 Core files (GLAD 2)...")
    execute_process(
            COMMAND "${VENV_PYTHON_EXE}" -m glad
            --api gl:core=4.6
            --out-path "${GLAD_OUT_DIR}"
            --reproducible
            c
            RESULT_VARIABLE GLAD_GEN_RESULT
    )
endif ()

# ==============================================================================
# 3. DEFINE GLAD LIBRARY
# ==============================================================================
set(GLAD_TYPE STATIC)
if (USE_SHARED_LIBS)
    set(GLAD_TYPE SHARED)
endif ()

add_library(glad ${GLAD_TYPE} "${GLAD_OUT_DIR}/src/gl.c")
# SYSTEM flag allows <glad/gl.h>
target_include_directories(glad SYSTEM PUBLIC "${GLAD_OUT_DIR}/include")

if (USE_SHARED_LIBS)
    target_compile_definitions(glad PUBLIC GLAD_GLAPI_EXPORT)
endif ()

# ==============================================================================
# 4. EXTERNAL DEPENDENCIES (GLFW, GLM, STB, IMGUI)
# ==============================================================================

# GLFW Setup
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS ${USE_SHARED_LIBS} CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# GLM (SYSTEM allows <glm/glm.hpp>)
add_library(glm INTERFACE)
target_include_directories(glm SYSTEM INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/external/glm")

# STB (SYSTEM allows <stb_image.h>)
add_library(stb INTERFACE)
target_include_directories(stb SYSTEM INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/external/stb")

# IMGUI SETUP (Manual integration because ImGui has no CMakeLists)
add_library(imgui INTERFACE)
target_include_directories(
        imgui SYSTEM INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui"
        "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends"
)

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")
add_library(imgui_backends STATIC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
# FIXED: Linked to 'glfw' (target from add_subdirectory) instead of 'GLFW::glfw'
target_link_libraries(imgui_backends PUBLIC imgui glfw glad)

# ==============================================================================
# 5. COMMON LIBS
# ==============================================================================
add_library(common_libs INTERFACE)
target_link_libraries(common_libs
        INTERFACE
        glad
        glfw
        glm
        stb
        imgui_backends
)

# ==============================================================================
# 6. HELPER FUNCTIONS
# ==============================================================================

function(silence_target target)
    if (TARGET ${target})
        get_target_property(target_type ${target} TYPE)
        if (NOT "${target_type}" STREQUAL "INTERFACE_LIBRARY")
            if (MSVC)
                target_compile_options(${target} PRIVATE /W0 /WX-)
            else ()
                target_compile_options(${target} PRIVATE -w -Wno-error)
            endif ()
        endif ()
    endif ()
endfunction()

silence_target(glad)
silence_target(glfw)
silence_target(imgui_backends)

function(create_lesson TARGET_NAME)
    if (BUILD_ALL_LESSONS)
        add_executable(${TARGET_NAME} src/main.cpp src/stb_define.cpp)
    else ()
        add_executable(${TARGET_NAME} EXCLUDE_FROM_ALL src/main.cpp src/stb_define.cpp)
    endif ()

    target_link_libraries(${TARGET_NAME} PRIVATE common_libs)

    # SYSTEM headers for user include directory
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/include")
        target_include_directories(${TARGET_NAME} SYSTEM PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/include")
    endif ()
    target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

    if (MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE /W4 /external:W3 /permissive- /GS /MP
                $<$<CONFIG:Debug>:/MDd /Od /RTC1>
                $<$<CONFIG:Release>:/MD /O2 /Oi /Gy /arch:AVX2>)
        target_compile_definitions(${TARGET_NAME} PRIVATE $<$<CONFIG:Release>:NDEBUG>)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic -fstack-protector-strong
                $<$<CONFIG:Debug>:-g>
                $<$<CONFIG:Release>:-O2 -march=native>)
    endif ()

    set_property(TARGET ${TARGET_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

    # ASSET COPYING LOGIC
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
        file(GLOB_RECURSE ASSET_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/*")
        foreach (ASSET_SOURCE_PATH ${ASSET_FILES})
            file(RELATIVE_PATH ASSET_REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${ASSET_SOURCE_PATH}")
            set(ASSET_DEST_PATH "${CMAKE_CURRENT_BINARY_DIR}/${ASSET_REL_PATH}")
            add_custom_command(
                    OUTPUT ${ASSET_DEST_PATH}
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASSET_SOURCE_PATH} ${ASSET_DEST_PATH}
                    DEPENDS ${ASSET_SOURCE_PATH}
            )
            list(APPEND COPIED_ASSETS ${ASSET_DEST_PATH})
        endforeach ()
        add_custom_target(copy_assets_${TARGET_NAME} ALL DEPENDS ${COPIED_ASSETS})
        add_dependencies(${TARGET_NAME} copy_assets_${TARGET_NAME})
    endif ()

    set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endfunction()

add_subdirectory(Projects)