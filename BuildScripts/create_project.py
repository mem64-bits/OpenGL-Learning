import os
import re
import shutil

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

# REGEX PATTERNS for file identification
RE_TEXT_FILES = re.compile(r".*\.(cpp|h|hpp|vert|frag|glsl)$", re.IGNORECASE)
RE_BINARY_FILES = re.compile(r".*\.(png|jpg|jpeg|tga|bmp)$", re.IGNORECASE)

# DIRECTORY REDIRECTIONS
# "Folder name in templates" : "Target folder name in new project"
DIR_REDIRECTS = {
    "shaders": "assets/shaders",
    "textures": "assets/textures",
    "include": "src/include",
    "src": "src",
}


def sanitize_name(name):
    # Enforces PascalCase without destroying existing capitalization
    words = re.split(r"[^a-zA-Z0-9]", name)
    return "".join(word[0].upper() + word[1:] for word in words if word)


def get_target_rel_path(template_rel_path):
    """
    Translates a template relative path to the project target relative path
    using the DIR_REDIRECTS map.
    """
    parts = template_rel_path.replace("\\", "/").split("/")
    root_dir = parts[0]

    if root_dir in DIR_REDIRECTS:
        parts[0] = DIR_REDIRECTS[root_dir]
        return "/".join(parts)

    return template_rel_path


def run(root_dir, cli_name=None, cli_category=None):
    print("Creating a New Project")
    print("-" * 30)

    # 1. Determine Project Name
    if cli_name:
        project_name = sanitize_name(cli_name)
        display_name = cli_name
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

    # 4. Generate Files from Templates (Dynamic Discovery via Regex)
    print("🔨 Generating files...")

    if not os.path.exists(TEMPLATES_DIR):
        print(f"Error: Templates folder not found at {TEMPLATES_DIR}")
        return

    # Crawl the templates directory
    for root, dirs, files in os.walk(TEMPLATES_DIR):
        for filename in files:
            # Full path to the template file
            src_template_path = os.path.join(root, filename)

            # Get path relative to the templates folder (e.g., 'shaders/shader.vert')
            rel_template_path = os.path.relpath(src_template_path, TEMPLATES_DIR)

            # Translate to target path (e.g., 'assets/shaders/shader.vert')
            rel_dest_path = get_target_rel_path(rel_template_path)
            dest_path = os.path.join(target_dir, rel_dest_path)

            # Create destination directory structure
            os.makedirs(os.path.dirname(dest_path), exist_ok=True)

            # Process Binary Files (Images)
            if RE_BINARY_FILES.match(filename):
                try:
                    shutil.copy2(src_template_path, dest_path)
                except IOError as e:
                    print(f"❌ Error copying binary {filename}: {e}")

            # Process Text Files (Code and Shaders)
            elif RE_TEXT_FILES.match(filename):
                try:
                    with open(src_template_path, "r", encoding="utf-8") as f_in:
                        content = f_in.read()

                    # Dynamic Replacement
                    content = content.replace("{{PROJECT_NAME}}", display_name)

                    with open(dest_path, "w", encoding="utf-8") as f_out:
                        f_out.write(content)
                except IOError as e:
                    print(f"❌ Error processing text template {filename}: {e}")

    print(f"Project created at: {target_dir}")

    # 5. Auto-Update CMake
    print("\nUpdating Build System...")
    gen_lesson_configs.run(root_dir)

    # Start from the parent folder of the new project and walk up to the root
    current_level = os.path.dirname(target_dir)
    root_abs = os.path.abspath(root_dir)

    while True:
        current_abs = os.path.abspath(current_level)
        if current_abs == root_abs:
            break

        rel_display = os.path.relpath(current_abs, root_abs)
        print(f"  Updating CMake: {rel_display}")

        gen_subdir_cmake.run(current_level)

        parent = os.path.dirname(current_level)
        if parent == current_level:
            break
        current_level = parent

    print("\n✅ Build system hierarchy updated! Ready to build.")


if __name__ == "__main__":
    run(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
