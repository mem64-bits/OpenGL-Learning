# OpenGL-Learning

## Requirements

* x86_64 CPU with AVX2 support (for optimization release profile)
* CMake version >= 3.25
* Visual Studio 2026 / MSVC
* Python 3.14.2 or greater

## Usage Guide

**Note:** (For all cmake commands on windows, commands will fail if not ran in VS Dev Prompt)

### 1. Clone repo with submodules

Clone repo with submodules needed for external libraries (glfw, glm, stb, etc.)

```bash
git clone --recursive https://github.com/mem64-bits/OpenGL-Learning
cd OpenGL-Learning
```

### 2. Install python 3.14.2 or higher to create venv environment to generate glad library files

#### Windows

If python is not installed get it from the official python
website [https://www.python.org/downloads/](https://www.python.org/downloads/)
(is confirmed to work on python 3.14.2 and above)

#### Linux

The preferred method to install python is through your distro's package manager. (Many distros include a python
installation by default, but package an older version of python3)

##### Debian / Ubuntu

Ubuntu comes installed with python but to install a specific version:

```bash
sudo apt install python<version-num> # python3.14
```

#### Fedora

Fedora comes pre-packaged with python but to get a more recent python version:

```bash
sudo dnf install python3
```

### 3. Generate CMake build configs

#### Windows

##### To make a debug profile for windows

```bash
cmake --preset msvc-windows-debug
```

##### To make a release build profile for windows

```bash
cmake --preset msvc-windows-release
```

#### Linux

```bash
cd OpenGL-Learning
```

##### To make a debug profile for Linux

```bash
cmake --preset gcc-linux-release
```

##### To make a release profile for Linux

```bash
# To make a release profile for Linux
cmake --preset gcc-linux-debug
```

### 4. Build Project in either Debug or Release profile

For Project Names enter the project folder name, located in `Projects/` that you want to build in PascalCase. All names
with spaces will be converted to PascalCase automatically e.g. "Shader Class" => "ShaderClass", when creating new
project through helper script.

#### Windows Debug Single Target Build

```bash
cmake --build .\out\build\windows-msvc-debug\ --target <ProjectName>
```

#### Windows Release Single Target Build

```bash
cmake --build .\out\build\windows-msvc-release\ --target <ProjectName>
```

#### Building All Projects for Windows

Ensure building all discovered projects options is selected in root `CMakeLists.txt` folder is set.

##### Make sure option is defined as "ON"

```cmake
option(BUILD_ALL_LESSONS "Build all discovered projects by default" ON)
```

##### Build all projects in Debug Profile

```bash
cmake --build .\out\build\windows-msvc-debug\
```

##### Build all projects in Release Profile

```bash
cmake --build .\out\build\windows-msvc-release\
```

#### Linux Debug Build (Single Project)

To build single project on linux-gcc-debug profile:

```bash
cmake --build out/build/linux-gcc-debug/ --target <Project_Name>
```

#### Linux Release Build (Single Project)

```bash
cmake --build out/build/linux-gcc-release/ --target <Project_Name>
```

#### Building All Projects for Linux

Ensure building all discovered projects options is selected in root `CMakeLists.txt` folder is set.

##### Make sure option is defined as "ON"

```cmake
option(BUILD_ALL_LESSONS "Build all discovered projects by default" ON)
```

Build all projects in debug mode:

```bash
cmake --build out/build/linux-gcc-debug/
```

Build all projects in release profile:

```bash
# Build all projects in release mode
cmake --build out/build/linux-gcc-release/
```

## Project Management Scripts

To streamline adding new lessons, managing CMake, and organizing folders, this project uses a custom Python script
`manage.py` to control the loading of python helper scripts (located in `BuildScripts/`) throughout the repo accessible
as an alias `git gl`.

### 1. Setup Git Alias

Run this command **once** in the repository root. This registers the `git gl` command, allowing you to run the helper
scripts from any folder.

Registers `git gl` to run the internal python management script:

```bash
git config alias.gl "!f() { python \"(git rev-parse --show-toplevel)/manage.py\" \"@\"; }; f"
```

### 2. Usage

#### Create a New Project

Automatically creates a new project folder populated with the C++ template files, assets, and headers.

* Enforces **PascalCase** for folder names (e.g. "spinning cube" -> `SpinningCube`).
* Keeps original spacing for the Window Title in C++.
* Automatically updates CMake configuration.

Creates `Projects/MyProject` populated with templates:

```bash
git gl new "My Project"
```

Creates in a specific category folder:

```bash
git gl new "My Project" -c Projects/Chapter1
```

#### Generate Lesson Configs

Scans the entire repository for `main.cpp` files. If a folder contains source code but no build config, this generates
the `CMakeLists.txt` for it automatically.

Scan and generate missing configs:

```bash
git gl gen-lessons
```

Force overwrite ALL lesson configs (Use if you renamed folders/targets):

```bash
git gl gen-lessons -f
```

#### Register Subdirectories

Updates the `CMakeLists.txt` in your **current directory** to include all immediate subfolders. Run this when you add a
new Chapter or Category folder.

Example: After adding a new folder `Projects/Chapter4`:

```bash
cd Projects
git gl gen-dirs
```