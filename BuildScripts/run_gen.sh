#!/bin/bash

# 1. Find the Repo Root
ROOT=$(git rev-parse --show-toplevel)

# 2. Navigate to the folder where the user typed the command
#    (Git sets GIT_PREFIX to the relative path of the current folder)
if [ -n "$GIT_PREFIX" ]; then
    cd "$ROOT/$GIT_PREFIX"
fi

# 3. Detect OS and run the correct script
OS=$(uname -s)

if [[ "$OS" == *"MINGW"* ]] || [[ "$OS" == *"CYGWIN"* ]] || [[ "$OS" == *"MSYS"* ]]; then
    # --- WINDOWS ---
    # Convert path to Windows format so PowerShell can find it
    PS_SCRIPT=$(cygpath -w "$ROOT/BuildScripts/add_cmake_subdirs.ps1")
    # Run PowerShell (It inherits the current directory from the 'cd' command above)
    powershell.exe -NoProfile -ExecutionPolicy Bypass -File "$PS_SCRIPT"
else
    # --- LINUX / MAC ---
    sh "$ROOT/BuildScripts/add_cmake_subdirs.sh"
fi