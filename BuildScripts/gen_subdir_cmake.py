import os
import subprocess

OUTPUT_FILE = "CMakeLists.txt"
IGNORE_LIST = {
    ".idea",
    ".obsidian",
    "build",
    "out",
    ".git",
    ".vs",
    ".vscode",
    "external",
    "BuildScripts",
    "cmake-build-debug",
    "__pycache__",
    "venv",
}


def get_git_root():
    try:
        root = subprocess.check_output(
            ["git", "rev-parse", "--show-toplevel"], stderr=subprocess.DEVNULL
        )
        return os.path.normpath(root.decode("utf-8").strip())
    except:
        return None


# ENTRY POINT FUNCTION
def run(target_dir):
    git_root = get_git_root()
    target_dir = os.path.normpath(target_dir)

    print(f"Scanning directory: {target_dir}")

    # Safety Check
    if git_root and target_dir == git_root:
        print("ABORTING: You are trying to run this on the Repo Root!")
        return

    # Scan
    try:
        all_items = os.listdir(target_dir)
    except FileNotFoundError:
        print(f"Error: Directory {target_dir} not found.")
        return

    valid_subdirs = []
    for item in all_items:
        full_path = os.path.join(target_dir, item)
        if (
            os.path.isdir(full_path)
            and item not in IGNORE_LIST
            and not item.startswith(".")
        ):
            valid_subdirs.append(item)

    valid_subdirs.sort()

    if not valid_subdirs:
        print("No valid subdirectories found.")
        return

    # Write
    content = ["# Generated Category Registry\n"]
    for subdir in valid_subdirs:
        content.append(f'add_subdirectory("{subdir}")\n')

    output_path = os.path.join(target_dir, OUTPUT_FILE)
    with open(output_path, "w", encoding="utf-8") as f:
        f.writelines(content)

    print(f"Generated {OUTPUT_FILE} with {len(valid_subdirs)} entries.")
