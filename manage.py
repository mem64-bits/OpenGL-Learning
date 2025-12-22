import sys
import os
import subprocess
import argparse


def main():
    script_root = os.path.dirname(os.path.abspath(__file__))

    # Handle Git Alias CWD logic
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
    subparsers.add_parser(
        "gen-dirs", help="Update CMakeLists.txt for subdirectories in current folder"
    )

    # Command: gen-lessons
    subparsers.add_parser(
        "gen-lessons", help="Scan repo and generate lesson CMake configs"
    )

    # ==========================================================================
    # COMMAND: new
    # ==========================================================================
    new_parser = subparsers.add_parser("new", help="Create a new project from template")

    # 1. Project Name (Optional positional argument)
    #    nargs='?' means "0 or 1 argument". If missing, it returns None.
    new_parser.add_argument(
        "name", nargs="?", help="Name of the project (e.g. 'My Game')"
    )

    # 2. Category / Location (Optional flag)
    #    defaults to None so we can detect if the user provided it or not
    new_parser.add_argument(
        "-c", "--category", help="Target folder (default: Projects)", default=None
    )

    args = parser.parse_args()

    # Dispatch
    if args.command == "gen-dirs":
        gen_subdir_cmake.run(user_cwd)
    elif args.command == "gen-lessons":
        gen_lesson_configs.run(script_root)
    elif args.command == "new":
        # Pass the arguments to the script
        create_project.run(script_root, args.name, args.category)
    else:
        parser.print_help()


if __name__ == "__main__":
    main()
