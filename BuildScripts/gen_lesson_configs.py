import os
import re
import subprocess

# ==============================================================================
# CONFIGURATION
# ==============================================================================
IGNORE_DIRS = {
    "external", "build", "out", "cmake-build-debug", ".git",
    "windows-msvc-debug", "linux-gcc-debug", "venv", "BuildScripts"
}

def sanitize_target_name(name):
    # If folder is "SpinningCube", this returns "SpinningCube" (Valid)
    # If folder is "My Game", this returns "My_Game" (Valid)
    return re.sub(r'[^a-zA-Z0-9_]', '_', name)

# UPDATED RUN SIGNATURE
def run(root_dir, force=False):
    print(f"🌱 Generating Lesson Configs starting from: {root_dir}")
    if force:
        print("   ⚠️  Force Mode Enabled: Overwriting existing CMakeLists.txt files.")

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
            target_name = sanitize_target_name(folder_name)
            content = f"create_lesson({target_name})\n"

            # Logic: Create if missing, OR overwrite if force is True
            if force or not os.path.exists(cmake_file):
                try:
                    mode = "Updated" if os.path.exists(cmake_file) else "Created"

                    with open(cmake_file, "w", encoding="utf-8") as f:
                        f.write(content)

                    print(f"   + {mode}: {folder_name} -> ({target_name})")

                    if mode == "Created": count_created += 1
                    else: count_updated += 1

                except IOError as e:
                    print(f"   ❌ Error writing to {folder_name}: {e}")

    print(f"✅ Processed lessons. Created {count_created} new, Updated {count_updated}.")

if __name__ == "__main__":
    # Default to False if run standalone
    run(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), force=False)