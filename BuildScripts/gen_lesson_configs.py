import os
import re

# ==============================================================================
# CONFIGURATION
# ==============================================================================
IGNORE_DIRS = {
    "external", "build", "out", "cmake-build-debug", ".git",
    "windows-msvc-debug", "linux-gcc-debug", "venv", "BuildScripts"
}

def sanitize_target_name(name):
    # FIXED: "CoordinateSystems" stays "CoordinateSystems"
    # Logic: Split by symbols, then uppercase ONLY the first letter of each part.
    # We avoid .capitalize() because it forces the rest of the word to lowercase.
    words = re.split(r'[^a-zA-Z0-9]', name)

    # Take first char upper + rest of the string as-is
    return "".join(word[0].upper() + word[1:] for word in words if word)

def run(root_dir, force=False):
    print(f"Generating Lesson Configs starting from: {root_dir}")
    if force:
        print("Force Mode Enabled: Overwriting existing CMakeLists.txt files.")

    count_created = 0
    count_updated = 0

    for current_root, dirs, files in os.walk(root_dir):
        # Prune ignore lists
        dirs[:] = [d for d in dirs if d not in IGNORE_DIRS and not d.startswith(".")]

        if "main.cpp" in files:
            src_dir = current_root
            lesson_dir = os.path.dirname(src_dir)
            folder_name = os.path.basename(lesson_dir)

            if lesson_dir == root_dir: continue

            cmake_file = os.path.join(lesson_dir, "CMakeLists.txt")

            # Apply PascalCase to the CMake Target Name
            target_name = sanitize_target_name(folder_name)

            content = f"create_lesson({target_name})\n"

            # Logic: Create if missing, OR overwrite if force is True
            if force or not os.path.exists(cmake_file):
                try:
                    mode = "Updated" if os.path.exists(cmake_file) else "Created"

                    with open(cmake_file, "w", encoding="utf-8") as f:
                        f.write(content)

                    print(f"   + {mode}: {folder_name} -> target: ({target_name})")

                    if mode == "Created": count_created += 1
                    else: count_updated += 1

                except IOError as e:
                    print(f"Error writing to {folder_name}: {e}")

    print(f"Processed lessons. Created {count_created} new, Updated {count_updated}.")

if __name__ == "__main__":
    run(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), force=False)