# We don't need cmake_minimum_required here because the Root handles it
project(GLFW-Window CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# 1. Sources
# -----------------------------------------------------------------------------
file(GLOB SOURCES "src/*.cpp" "src/*.cxx" "src/*.c++")
add_executable(${PROJECT_NAME} ${SOURCES})

# -----------------------------------------------------------------------------
# 2. Linking
# -----------------------------------------------------------------------------
# We link against the names we defined in the Root
target_link_libraries(${PROJECT_NAME} PRIVATE
    glad        # The 4.6 loader
    glfw        # Windowing
    glm::glm    # Math
    stb         # Images
)

# -----------------------------------------------------------------------------
# 3. Asset Copying (Automatic)
# -----------------------------------------------------------------------------
# This finds everything in your 'assets' folder and copies it to the build folder
file(GLOB_RECURSE ASSET_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/*")

foreach(ASSET_SOURCE_PATH ${ASSET_FILES})
    # Calculate the relative path (e.g., "shaders/vertex.glsl")
    file(RELATIVE_PATH ASSET_REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/assets" "${ASSET_SOURCE_PATH}")
    
    # Determine where it goes in the build folder
    set(ASSET_DEST_PATH "${CMAKE_CURRENT_BINARY_DIR}/assets/${ASSET_REL_PATH}")

    # Create the copy command
    add_custom_command(
        OUTPUT ${ASSET_DEST_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASSET_SOURCE_PATH} ${ASSET_DEST_PATH}
        DEPENDS ${ASSET_SOURCE_PATH}
        COMMENT "Copying asset: ${ASSET_REL_PATH}"
    )
    
    # Track this file
    list(APPEND COPIED_ASSETS ${ASSET_DEST_PATH})
endforeach()

# Create a target so CMake knows to run the copy commands
add_custom_target(copy_assets_opengl ALL DEPENDS ${COPIED_ASSETS})
add_dependencies(${PROJECT_NAME} copy_assets_opengl)