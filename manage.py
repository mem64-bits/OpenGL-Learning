import sys
import os
import subprocess
import argparse

def main():
    script_root = os.path.dirname(os.path.abspath(__file__))

    # Git Alias Logic
    git_relative_path = os.environ.get("GIT_PREFIX", "")
    user_cwd = os.path.abspath(os.path.join(os.getcwd(), git_relative_path))

    sys.path.append(script_root)

    try:
        from BuildScripts import gen_subdir_cmake
        from BuildScripts import gen_lesson_configs
        from BuildScripts import create_project
    except ImportError as e:
        print(f"❌ Error importing BuildScripts: {e}")
        return

    parser = argparse.ArgumentParser(description="OpenGL Project Manager")
    subparsers = parser.add_subparsers(dest="command", help="Available commands")

    # Command: gen-dirs
    subparsers.add_parser("gen-dirs", help="Update CMakeLists.txt for subdirectories in current folder")

    # Command: gen-lessons
    gen_lessons_parser = subparsers.add_parser("gen-lessons", help="Scan repo and generate lesson CMake configs")
    # NEW FLAG: -f / --force
    gen_lessons_parser.add_argument("-f", "--force", action="store_true", help="Overwrite existing CMakeLists.txt files")

    # Command: new
    new_parser = subparsers.add_parser("new", help="Create a new project from template")
    new_parser.add_argument("name", nargs="?", help="Name of the project")
    new_parser.add_argument("-c", "--category", help="Target folder inside Projects/")

    args = parser.parse_args()

    if args.command == "gen-dirs":
        gen_subdir_cmake.run(user_cwd)
    elif args.command == "gen-lessons":
        # Pass the force argument
        gen_lesson_configs.run(script_root, force=args.force)
    elif args.command == "new":
        create_project.run(script_root, args.name, args.category)
    else:
        parser.print_help()

if __name__ == "__main__":
    main()