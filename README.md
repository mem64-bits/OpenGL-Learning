# OpenGL-Learning

## Requirements

* x86_64 CPU with AVX2 support (required for the Optimized Release profile)
* CMake version >= 3.25
* Visual Studio 2026 / MSVC (Windows) or GCC 15+ (Linux)
* Python 3.14.2 or greater (required for GLAD2 generation)

## Usage Guide

**Note:** On Windows, all CMake commands must be executed within the **Visual Studio Developer Command Prompt** to ensure the environment variables are correctly mapped.

### 1. Clone repo with submodules

Clone the repository with submodules required for external libraries (GLFW, GLM, STB, ImGui, etc.).

```bash
git clone --recursive https://github.com/mem64-bits/OpenGL-Learning
cd OpenGL-Learning
```

### 2. Install Project Dependencies

#### Windows
If Python is not installed, download it from the official website: [https://www.python.org/downloads/](https://www.python.org/downloads/). All other dependencies are bundled in `/external` via git submodules.

#### Linux
System-level headers must be installed to support native Wayland/EGL rendering and the required libraries for linking the build suite.

##### Fedora
The following command installs the build suite, Wayland/X11 development headers, and the static sanitizers required for the `linux-debug` profile.

```bash
sudo dnf install \
    gcc-c++ cmake ninja-build python3 \
    wayland-devel libxkbcommon-devel libdecor-devel \
    libX11-devel libXcursor-devel libXi-devel libXinerama-devel libXrandr-devel \
    mesa-libGL-devel mesa-libEGL-devel \
    libasan-static libubsan-static
```

##### Debian / Ubuntu
```bash
sudo apt update
sudo apt install \
    build-essential cmake ninja-build python3-venv \
    libwayland-dev libxkbcommon-dev libdecor-0-dev \
    libx11-dev libxcursor-dev libxi-dev libxinerama-dev libxrandr-dev \
    libgl1-mesa-dev libegl1-mesa-dev \
    libasan8 libubsan1
```

### 3. Generate CMake Build Configs

#### Windows (VS Dev Prompt)
```bash
# Generate Debug Profile
cmake --preset windows-msvc-debug

# Generate Release Profile
cmake --preset windows-msvc-release
```

#### Linux
```bash
# Generate Debug Profile
cmake --preset linux-debug

# Generate Release Profile
cmake --preset linux-release
```

### 4. Building Projects

When building single targets, enter the folder name located in `Projects/` in PascalCase.

#### Windows Build (Single Target)
```bash
# Debug
cmake --build out\build\windows-msvc-debug --target <ProjectName>

# Release
cmake --build out\build\windows-msvc-release --target <ProjectName>
```

#### Linux Build (Single Target)
```bash
# Debug
cmake --build out/build/linux-debug --target <ProjectName>

# Release
cmake --build out/build/linux-release --target <ProjectName>
```

#### Building All Projects
Ensure `option(BUILD_ALL_LESSONS "ON")` is set in the root `CMakeLists.txt`.

```bash
# Windows
cmake --build out\build\windows-msvc-debug

# Linux
cmake --build out/build/linux-debug
```

## Project Management Scripts

This project uses a custom Python management script `manage.py` accessible via the git alias `git gl`.

### 1. Setup Git Alias
Run this command **once** in the repository root to register the management command:

```bash
git config alias.gl "!f() { python \"$(git rev-parse --show-toplevel)/manage.py\" \"$@\"; }; f"
```

### 2. Usage

#### **Create a New Project**
Creates a new project folder populated with C++ template files, assets, and shaders.
(Template files are located in `BuildScripts/templates`)
```bash
# Creates Projects/MyProject
git gl new "My Project"

# Creates in a specific category folder
git gl new "My Project" -c Projects/Chapter1
```

#### **Generate Lesson Configs**
Scans the entire repository for `main.cpp` files and generates missing `CMakeLists.txt` files.
```bash
git gl gen-lessons
```

#### **Register Subdirectories**
Updates the local `CMakeLists.txt` to include all immediate subfolders as build targets.
```bash
cd Projects
git gl gen-dirs
```