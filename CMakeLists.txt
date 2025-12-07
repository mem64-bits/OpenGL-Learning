cmake_minimum_required(VERSION 3.25)
project(OpenGL-Learning CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fix for Windows headers breaking std::min/max
if(MSVC)
    add_compile_definitions(NOMINMAX)
endif()

# ==============================================================================
#  USER OPTIONS
# ==============================================================================

option(USE_SHARED_LIBS "Build external libraries as Shared (Dynamic) instead of Static" OFF)
option(BUILD_ALL_LESSONS "Build all discovered projects by default" ON)

# Links glad dynamically or statically depending on option chosen 
if(USE_SHARED_LIBS)
    set(GLAD_TYPE SHARED)
else()
    set(GLAD_TYPE STATIC)
endif()

add_subdirectory(external/glad/cmake) 
glad_add_library(glad ${GLAD_TYPE} REPRODUCIBLE API gl:core=4.6)

# GLFW Setup
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Links GLFW with set linking method
set(BUILD_SHARED_LIBS ${USE_SHARED_LIBS} CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)

# Defines GLM Library (Header Only)
add_library(glm INTERFACE)
target_include_directories(glm SYSTEM INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/external/glm")

# Defines STB Library (Header Only) ---
add_library(stb INTERFACE)
target_include_directories(stb SYSTEM INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/external/stb")

# Setups setup the common libraries project will be linked against
add_library(common_libs INTERFACE)
target_link_libraries(common_libs
    INTERFACE
        glad
        glfw
        glm
        stb
)


# Function to suppress warnings for external libraries
function(silence_target target)
    if(TARGET ${target})
        get_target_property(target_type ${target} TYPE)
        if(NOT "${target_type}" STREQUAL "INTERFACE_LIBRARY")
            if(MSVC)
                target_compile_options(${target} PRIVATE /W0 /WX-)
            else()
                target_compile_options(${target} PRIVATE -w -Wno-error)
            endif()
        endif()
    endif()
endfunction()

# Stops library code from generating warnings to focus on user caused problems
silence_target(glad)
silence_target(glfw)

# Function to define a lesson project executable
function(create_lesson TARGET_NAME)
    if(BUILD_ALL_LESSONS)
        add_executable(${TARGET_NAME} src/main.cpp)
    else()
        add_executable(${TARGET_NAME} EXCLUDE_FROM_ALL src/main.cpp)
    endif()
    # Links every project with the commonly used libraries
    target_link_libraries(${TARGET_NAME} PRIVATE common_libs)

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/include")
        target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/include")
    endif()
    # Add 'src' to path so templates work
    target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

    # COMPILER FLAGS & OPTIMIZATIONS
    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE 
            /W4 /external:W3 /permissive- /GS /MP
            $<$<CONFIG:Debug>:/MDd /Od /RTC1>
            $<$<CONFIG:Release>:/MD /O2 /Oi /Gy /arch:AVX2>
        )
        target_compile_definitions(${TARGET_NAME} PRIVATE $<$<CONFIG:Release>:NDEBUG>)
   # Sets up linux options
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(${TARGET_NAME} PRIVATE 
            -Wall -Wextra -Wpedantic -fstack-protector-strong
            $<$<CONFIG:Debug>:-g>
            $<$<CONFIG:Release>:-O2 -march=native>
        )
    endif()

    # Enable Interprocedural Optimization (LTO) for Release
    set_property(TARGET ${TARGET_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

    # 5. AUTO-COPY ASSETS
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
        file(GLOB_RECURSE ASSET_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/*")
        set(COPIED_ASSETS "")

        foreach(ASSET_SOURCE_PATH ${ASSET_FILES})
            file(RELATIVE_PATH ASSET_REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${ASSET_SOURCE_PATH}")
            set(ASSET_DEST_PATH "${CMAKE_CURRENT_BINARY_DIR}/${ASSET_REL_PATH}")

            add_custom_command(
                OUTPUT ${ASSET_DEST_PATH}
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASSET_SOURCE_PATH} ${ASSET_DEST_PATH}
                DEPENDS ${ASSET_SOURCE_PATH}
                COMMENT "📦 Updating asset: ${ASSET_REL_PATH}"
            )
            list(APPEND COPIED_ASSETS ${ASSET_DEST_PATH})
        endforeach()

        add_custom_target(copy_assets_${TARGET_NAME} ALL DEPENDS ${COPIED_ASSETS})
        add_dependencies(${TARGET_NAME} copy_assets_${TARGET_NAME})
    endif()

    # 6. AUTO-COPY DLLS (Only if USE_SHARED_LIBS is ON)
    if(WIN32 AND USE_SHARED_LIBS)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${TARGET_NAME}>
            $<TARGET_FILE_DIR:${TARGET_NAME}>
            COMMAND_EXPAND_LISTS
            COMMENT "📚 Copying Dynamic Libraries..."
        )
    endif()

    # Sets Debug Directory to where the executable is
    set_target_properties(${TARGET_NAME} PROPERTIES 
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
endfunction()

# begins pointing to Project location to begin CMakeLists project chain
add_subdirectory(Projects)