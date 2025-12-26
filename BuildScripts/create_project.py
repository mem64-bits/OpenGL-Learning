import os
import re
import shutil  # Added for safer binary copying

try:
    from BuildScripts import gen_lesson_configs
    from BuildScripts import gen_subdir_cmake
except ImportError:
    import gen_lesson_configs
    import gen_subdir_cmake

DEFAULT_CATEGORY = "Projects"

# Get the path to the 'templates' folder relative to this script
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
TEMPLATES_DIR = os.path.join(SCRIPT_DIR, "templates")

# Map logic: "Output Path inside Project": "Template Filename"
# NOTICE: Removed engine-specific headers/cpps as they now live in /core
FILE_MAPPINGS = {
    "src/main.cpp": "src/main.cpp",
    "assets/shaders/shader.vert": "shaders/shader.vert",
    "assets/shaders/shader.frag": "shaders/shader.frag",
    "assets/textures/fire.png": "textures/fire.png",
    "assets/textures/wall.jpg": "textures/wall.jpg",
}


def sanitize_name(name):
    # FIXED: Enforces PascalCase without destroying existing capitalization
    # Example: "Coordinate Systems" -> "CoordinateSystems"
    # Example: "CoordinateSystems" -> "CoordinateSystems"
    words = re.split(r"[^a-zA-Z0-9]", name)
    return "".join(word[0].upper() + word[1:] for word in words if word)


def run(root_dir, cli_name=None, cli_category=None):
    print("Creating a New Project")
    print("-" * 30)

    # 1. Determine Project Name
    if cli_name:
        project_name = sanitize_name(cli_name)
        display_name = cli_name  # Keep the original spacing for Window Title
        print(f"🔹 Name: {project_name}")
    else:
        raw_name = input("Enter Project Name (e.g. 'Spinning Cube'): ")
        if not raw_name:
            return
        project_name = sanitize_name(raw_name)
        display_name = raw_name

    # 2. Determine Category
    if cli_category:
        raw_loc = cli_category
    elif cli_name:
        print(f"🔹 Category: {DEFAULT_CATEGORY} (Default)")
        raw_loc = DEFAULT_CATEGORY
    else:
        raw_loc = input(f"Enter Category Folder (Default: '{DEFAULT_CATEGORY}'): ")
        if not raw_loc.strip():
            raw_loc = DEFAULT_CATEGORY

    # 3. Path Setup
    target_dir = os.path.join(root_dir, raw_loc, project_name)
    target_dir = os.path.normpath(target_dir)

    if os.path.exists(target_dir):
        print(f"Error: Folder already exists: {target_dir}")
        return

    # 4. Generate Files from Templates
    print("🔨 Generating files...")

    if not os.path.exists(TEMPLATES_DIR):
        print(f"Error: Templates folder not found at {TEMPLATES_DIR}")
        return

    for rel_path, template_name in FILE_MAPPINGS.items():
        dest_path = os.path.join(target_dir, rel_path)
        src_template = os.path.join(TEMPLATES_DIR, template_name)

        # Create directory if needed
        os.makedirs(os.path.dirname(dest_path), exist_ok=True)

        # Check if a source exists
        if not os.path.exists(src_template):
            print(f"Warning: Template '{template_name}' missing. Skipping.")
            continue

        # FIXED: Binary File Handling
        # If it's an image, just copy bytes. If texted, replace placeholders.
        if template_name.lower().endswith((".png", ".jpg", ".jpeg", ".tga", ".bmp")):
            try:
                shutil.copy2(src_template, dest_path)
            except IOError as e:
                print(f"❌ Error copying image {template_name}: {e}")
        else:
            # Text Processing
            try:
                with open(src_template, "r", encoding="utf-8") as f_in:
                    content = f_in.read()

                # Dynamic Replacement
                content = content.replace("{{PROJECT_NAME}}", display_name)

                with open(dest_path, "w", encoding="utf-8") as f_out:
                    f_out.write(content)
            except IOError as e:
                print(f"Error processing template {template_name}: {e}")

    print(f"Project created at: {target_dir}")

    # 5. Auto-Update CMake
    # FIXED: Implements recursive upward walk but STOPS before the root
    print("\nUpdating Build System...")

    # Refresh the global lesson configurations
    gen_lesson_configs.run(root_dir)

    # Start from the parent folder of the new project
    current_level = os.path.dirname(target_dir)
    root_abs = os.path.abspath(root_dir)

    while True:
        current_abs = os.path.abspath(current_level)

        # STOP CRITICAL: Do not run subdir_cmake on the root directory.
        # The root has the Master CMakeLists.txt and must be protected.
        if current_abs == root_abs:
            break

        # Display progress relative to root
        rel_display = os.path.relpath(current_abs, root_abs)
        print(f"  Updating CMake: {rel_display}")

        # Generate/Update the CMakeLists.txt for this specific folder
        gen_subdir_cmake.run(current_level)

        # Move one folder up
        parent = os.path.dirname(current_level)
        if parent == current_level:  # Safety break for drive root
            break
        current_level = parent

    print("\n✅ Build system hierarchy updated! Ready to build.")


if __name__ == "__main__":
    # Standard entry point: gets the root directory relative to the script location
    run(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
