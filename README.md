# OpenGL-Learning

## Requirements

- x86_64 CPU with AVX2 support (for optimization release profile)
- CMake version >= 3.25
- Visual Studio 2026 / MSVC
- Python 3.14.2 or greater

## Usage Guide

###### Note: (For all cmake commands on windows, commands will fail if not ran in VS Dev Prompt)
### 1. Clone repo with submodules

```bash
# git clone repo with submodules needed for external libraries (glfw, glm, stb, etc.) 

git clone --recursive https://github.com/mem64-bits/OpenGL-Learning
```

### 2. Install python 3.14.2 or higher to create venv environment to generate glad library files


### Windows

 **if python is not installed get it from the official python website**
[https://www.python.org/downloads/](https://www.python.org/downloads/)

(is confirmed to work on python 3.14.2 and above)

### Linux 

The preferred method to install python is through your distro's package manager.
(Many distros include a python installation by default, but package an older version of python3 )

#### Debian / Ubuntu

```bash
#Ubuntu comes installed with python but to install a specific version 
sudo apt install python<version-num> # python3.14
```

#### Fedora

```bash
# Fedora comes pre-packaged with python but to get a more recent python version

sudo dnf install python3
```


### 3. Generate CMake build configs

#### Windows 
```bash
cd OpenGL-Learning

# To make a debug profile for windows
cmake --preset msvc-windows-debug

# To make a release build profile for windows
cmake --preset msvc-windows-release

```

#### Linux 
```bash

cd OpenGL-Learning
# To make a debug profile for linux
cmake --preset gcc-linux-release

# To make a release profile for linux
cmake --preset gcc-linux-debug
```
### 4. Build Project in either Debug or Release profile

For Project Names enter the project folder name, located in `Projects/` that you want to build but replace any spaces with "_" .
e.g. "Shader Class" => "Shader_Class".
(Folders with no spaces remain the same) e.g Uniforms => Uniforms


#### Windows

##### Windows Debug Build
``` bash
# Run in root of git repo
# To build single project on msvc-windows-debug profile
cmake --build .\out\build\windows-msvc-debug\ --target <Project_Name>
```

##### Windows Release Build
```bash
# Run in root of git repo
# To build single project on msvc-windows-debug profile
cmake --build .\out\build\windows-msvc-release\ --target <Project_Name>
```

##### Building All Projects for Windows

###### Ensure building all discovered projects options is selected in root CMakeLists.txt folder is set 
```cmake
# Make sure option is defined as "ON"
option(BUILD_ALL_LESSONS "Build all discovered projects by default" ON)
```

##### Build all projects in debug profile
```bash
# Build all projects in debug mode
cmake --build .\out\build\windows-msvc-debug\ 
```

##### Build all projects in release profile
```bash
# Build all projects in release mode
cmake --build .\out\build\windows-msvc-debug\ 
```


#### Linux 

##### Linux Debug Build
``` bash
# Run in root of git repo
# To build single project on linux-gcc-debug profile
cmake --build out/build/linux-gcc-debug/ --target <Project_Name>
```

##### Linux Release Build
```bash
# Run in root of git repo
# To build single project on linux-gcc-release profile
cmake --build out/build/linux-gcc-release/ --target <Project_Name>
```

##### Building All Projects for Linux

###### Ensure building all discovered projects options is selected in root CMakeLists.txt folder is set 
```cmake
# Make sure option is defined as "ON"
option(BUILD_ALL_LESSONS "Build all discovered projects by default" ON)
```

##### Build all projects in debug profile
```bash
# Build all projects in debug mode
cmake --build out/build/linux-gcc-debug/
```

##### Build all projects in release profile
```bash
# Build all projects in release mode
cmake --build out/build/linux-gcc-release/ 
```


## Useful Scripts and Git Aliases

To make adding new projects to the setup easier, scripts in the `BuildScripts/` folder have been created to generate the subdirectories CmakeLists.txt Project folders, and another script to create a new project lesson.

These can be called by running....

```bash
cd Projects/
git gen-cmake
```

```bash
git gen-lessons
```

(To be expanded upon in future commit)
