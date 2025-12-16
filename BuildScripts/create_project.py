import os
import sys
import re

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

# Map logic: "Output Path inside Project" : "Template Filename"
# This tells the script where to put the files and which template to use.
FILE_MAPPINGS = {
    "src/main.cpp": "main.cpp",
    "src/stb_define.cpp": "stb_define.cpp",
    "src/include/glfwCallBacks.h": "glfwCallBacks.h",
    "src/include/Shader.h": "Shader.h",
    "assets/shaders/shader.vert": "shader.vert",
    "assets/shaders/shader.frag": "shader.frag"
}

def sanitize_name(name):
    name = name.strip().replace(" ", "_")
    return re.sub(r'[^a-zA-Z0-9_]', '', name)

def run(root_dir, cli_name=None, cli_category=None):
    print("✨ Creating a New Project")
    print("-" * 30)

    # 1. Determine Project Name (Same as before)
    if cli_name:
        project_name = sanitize_name(cli_name)
        display_name = cli_name # Keep the original spaces for the Window Title!
        print(f"🔹 Name: {project_name}")
    else:
        raw_name = input("Enter Project Name (e.g. 'Spinning Cube'): ")
        if not raw_name: return
        project_name = sanitize_name(raw_name)
        display_name = raw_name

    # 2. Determine Category (Same as before)
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
        print(f"❌ Error: Folder already exists: {target_dir}")
        return

    # 4. Generate Files from Templates
    print("🔨 Generating files...")

    # Check if templates exist
    if not os.path.exists(TEMPLATES_DIR):
        print(f"❌ Error: Templates folder not found at {TEMPLATES_DIR}")
        return

    for rel_path, template_name in FILE_MAPPINGS.items():
        # A. Determine full destination path
        dest_path = os.path.join(target_dir, rel_path)

        # B. Determine full template path
        src_template = os.path.join(TEMPLATES_DIR, template_name)

        # C. Create directory if needed
        os.makedirs(os.path.dirname(dest_path), exist_ok=True)

        try:
            with open(src_template, "r") as f_in:
                content = f_in.read()

            # D. Dynamic Replacement
            # Replace {{PROJECT_NAME}} with the actual name (e.g. "Spinning Cube")
            content = content.replace("{{PROJECT_NAME}}", display_name)

            with open(dest_path, "w") as f_out:
                f_out.write(content)

        except FileNotFoundError:
            print(f"⚠️  Warning: Template '{template_name}' missing. Skipping.")

    print(f"✅ Project created at: {target_dir}")

    # 5. Auto-Update CMake
    print("\n⚙️  Updating Build System...")
    gen_lesson_configs.run(root_dir)
    parent_dir = os.path.dirname(target_dir)
    gen_subdir_cmake.run(parent_dir)

    print("\n🚀 Ready to build!")

if __name__ == "__main__":
    run(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))