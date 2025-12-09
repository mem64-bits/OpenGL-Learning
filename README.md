# OpenGL-Learning

## Requirements

- x86_64 CPU with AVX2 support (for optimization release profile)
- CMake version >= 3.25
- Visual Studio 2026 / MSVC
- Python 3.14.2 or greater

## Usage Guide

### 1. Clone repo with submodules

```bash
# git clone repo with submodules needed for external libraries (glfw, glm, stb, etc.) 

git clone --recursive https://github.com/mem64-bits/OpenGL-Learning
cd OpenGL-Learning
```

#### 2. Install python 3.14.2 or higher to create venv environment to generate glad library files


![[Windows_11_logo2.svg||64]]
### Windows

 **if python is not installed get it from the official python website**
[https://www.python.org/downloads/](https://www.python.org/downloads/)

(is confirmed to work on python 3.14.2 and above)


 ![[linux-tux-1.svg|78x78]]
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


![[Windows_11_logo2.svg||64]]
#### Windows 
```bash
cd OpenGL-Learning

# To make a debug profile for windows
cmake --preset msvc-windows-debug

# To make a release build profile for windows
cmake --preset msvc-windows-release

```


 ![[linux-tux-1.svg|78x78]]
#### Linux 

```bash

cd OpenGL-Learning
# To make a debug profile for linux
cmake --preset gcc-linux-release

# To make a release profile for linux
cmake --preset gcc-linux-debug
```