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


#  PYTHON VENV & GLAD GENERATION
find_package(Python3 COMPONENTS Interpreter REQUIRED)
set(VENV_DIR "${CMAKE_BINARY_DIR}/venv")

if (WIN32)
    set(VENV_PYTHON_EXE "${VENV_DIR}/Scripts/python.exe")
else ()
    set(VENV_PYTHON_EXE "${VENV_DIR}/bin/python3")
endif ()

if (NOT EXISTS "${VENV_PYTHON_EXE}")
    message(STATUS "Creating Python Virtual Environment for GLAD...")
    execute_process(COMMAND "${Python3_EXECUTABLE}" -m venv "${VENV_DIR}")
    execute_process(COMMAND "${VENV_PYTHON_EXE}" -m pip install --upgrade pip glad2 jinja2 --quiet)
endif ()

set(GLAD_OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glad")
set(GLAD_HEADER "${GLAD_OUT_DIR}/include/glad/gl.h")

if (NOT EXISTS "${GLAD_HEADER}")
    message(STATUS "Generating OpenGL 4.6 Core + EGL (GLAD 2)...")
    execute_process(COMMAND "${VENV_PYTHON_EXE}" -m glad
            --api gl:core=4.6,egl
            --out-path "${GLAD_OUT_DIR}"
            --reproducible c)
endif ()

# Compiles glad statically or dynamically depending on flag
set(GLAD_TYPE STATIC)
if (USE_SHARED_LIBS)
    set(GLAD_TYPE SHARED)
endif ()

add_library(glad ${GLAD_TYPE} "${GLAD_OUT_DIR}/src/gl.c")
target_include_directories(glad SYSTEM PUBLIC "${GLAD_OUT_DIR}/include")

# GLFW NATIVE WAYLAND SETUP
set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE)   # Native Wayland
set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)       # Fallback X11
set(GLFW_USE_LIBDECOR ON CACHE BOOL "" FORCE)    # Window Borders
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# --- GLM & STB (Header Only) ---
add_library(glm INTERFACE)
target_include_directories(glm SYSTEM INTERFACE "${CMAKE_SOURCE_DIR}/external/glm")

add_library(stb INTERFACE)
target_include_directories(stb SYSTEM INTERFACE "${CMAKE_SOURCE_DIR}/external/stb")

# --- IMGUI SETUP ---
add_library(imgui_headers INTERFACE)
target_include_directories(imgui_headers SYSTEM INTERFACE
        "${CMAKE_SOURCE_DIR}/external/imgui"
        "${CMAKE_SOURCE_DIR}/external/imgui/backends"
)

add_library(imgui_backends STATIC
        "${CMAKE_SOURCE_DIR}/external/imgui/imgui.cpp"
        "${CMAKE_SOURCE_DIR}/external/imgui/imgui_draw.cpp"
        "${CMAKE_SOURCE_DIR}/external/imgui/imgui_widgets.cpp"
        "${CMAKE_SOURCE_DIR}/external/imgui/imgui_tables.cpp"
        "${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_glfw.cpp"
        "${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3.cpp"
)
target_link_libraries(imgui_backends PUBLIC imgui_headers PRIVATE glfw glad)

# --- STB IMPLEMENTATION ---
add_library(stb_impl STATIC "${CMAKE_SOURCE_DIR}/core/src/stb_define.cpp")
target_link_libraries(stb_impl PRIVATE stb)

# ==============================================================================
#  CORE ENGINE LIBRARY
# ==============================================================================
set(CORE_SOURCES
        "${CMAKE_SOURCE_DIR}/core/src/Window.cpp"
        "${CMAKE_SOURCE_DIR}/core/src/Camera.cpp"
        "${CMAKE_SOURCE_DIR}/core/src/FPSCounter.cpp"
        "${CMAKE_SOURCE_DIR}/core/src/Shader.cpp"
        "${CMAKE_SOURCE_DIR}/core/src/Texture.cpp"
        "${CMAKE_SOURCE_DIR}/core/src/ImageLoader.cpp"
)

add_library(core STATIC ${CORE_SOURCES})
target_include_directories(core PUBLIC "${CMAKE_SOURCE_DIR}/core/src")

target_link_libraries(core PUBLIC glad glfw glm stb stb_impl imgui_backends)

if (UNIX AND NOT APPLE)
    # Required for Linux dynamic loading, math, and threads
    target_link_libraries(core PUBLIC m dl pthread)
endif ()

# ==============================================================================
#  HELPER TARGETS & COMPILER CONFIGURATION
# ==============================================================================
add_library(common_libs INTERFACE)
target_link_libraries(common_libs INTERFACE core)

add_library(common_libs_old INTERFACE)
target_link_libraries(common_libs_old INTERFACE glad glfw glm stb stb_impl)

function(silence_target target)
    if (TARGET ${target})
        if (MSVC)
            target_compile_options(${target} PRIVATE /W0 /WX-)
        else ()
            target_compile_options(${target} PRIVATE -w -Wno-error)
        endif ()
    endif ()
endfunction()

silence_target(glad)
silence_target(glfw)
silence_target(imgui_backends)
silence_target(stb_impl)


#  LESSON CREATION FUNCTION
function(create_lesson TARGET_NAME)
    # LEGACY flag makes old projects compatible with engine build system
    set(options LEGACY)
    cmake_parse_arguments(ARG "${options}" "" "" ${ARGN})

    if (BUILD_ALL_LESSONS)
        add_executable(${TARGET_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    else ()
        add_executable(${TARGET_NAME} EXCLUDE_FROM_ALL "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    endif ()

    if (ARG_LEGACY)
        target_link_libraries(${TARGET_NAME} PRIVATE common_libs_old)
        target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src")
    else ()
        target_link_libraries(${TARGET_NAME} PRIVATE common_libs)
    endif ()

    if (MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE
                /W4 /external:W3 /permissive- /GS /MP /Zc:__cplusplus /Zc:inline /Zc:preprocessor /utf-8
                /diagnostics:caret /sdl
                $<$<CONFIG:Release>:/O2 /Oi /Gy /arch:AVX2 /GL /Gw /INCREMENTAL:NO>
                $<$<CONFIG:Debug>:/Zi /RTC1 /guard:cf>)

        target_link_options(${TARGET_NAME} PRIVATE
                $<$<CONFIG:Release>:/LTCG /OPT:REF /OPT:ICF>)

        target_compile_definitions(${TARGET_NAME} PRIVATE $<$<CONFIG:Release>:NDEBUG>)
        set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS_DEBUG "/DEBUG")

        # Linux compile flags
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${TARGET_NAME} PRIVATE
                -Wall -Wextra             # The essentials
                -Wsign-compare            # int vs unsigned (crucial for .size() calls)
                -Wsign-conversion         # Catch accidental negative to positive conversion
                -Wshadow
                -Wdouble-promotion        # Catch accidental 'double' usage (OpenGL performance)
                -Wformat=2                # Better checks for strings/logging
                -Wimplicit-fallthrough    # Warns if you forget a 'break' in a switch
                -Wnull-dereference        # Catch obvious null pointer paths

                -Wno-unused-parameter     # stops annoying glfw callback unused param messages
                -Wno-missing-field-initializers # Relaxed struct init

                # Warning Formatting
                -fdiagnostics-color=always
                -fdiagnostics-show-option  # Shows exactly WHICH flag caused the warning so you can toggle it
                -fdiagnostics-show-caret   # Points exactly to the error column

                # arranges data and functions, to allow dead code removal
                -ffunction-sections
                -fdata-sections

                # Flags specific to Debug and Release builds
                $<$<CONFIG:Release>:-O3 -mavx2 -mfma -flto=auto -ftree-vectorize -fno-math-errno>
                $<$<CONFIG:Debug>:-g3 -Og -D_GLIBCXX_ASSERTIONS -fno-omit-frame-pointer -fsanitize=address,undefined>)

        target_link_options(${TARGET_NAME} PRIVATE
                -Wl,--as-needed
                $<$<CONFIG:Release>:-flto=auto -Wl,--gc-sections>
                $<$<CONFIG:Debug>:-fsanitize=address,undefined>)

        target_compile_definitions(${TARGET_NAME} PRIVATE $<$<CONFIG:Release>:NDEBUG>)
    endif ()

    # Copies shaders and assets to build folder smartly only on changes
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
        file(GLOB_RECURSE ASSET_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/*")
        set(COPIED_ASSETS "")
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