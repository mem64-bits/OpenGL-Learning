#!/bin/bash

ROOT=$(git rev-parse --show-toplevel)
OS=$(uname -s)

if [[ "$OS" == *"MINGW"* ]] || [[ "$OS" == *"CYGWIN"* ]] || [[ "$OS" == *"MSYS"* ]]; then
    # --- WINDOWS (Run PowerShell) ---
    # Convert path to Windows format so PowerShell accepts it
    WIN_PATH=$(cygpath -w "$ROOT/BuildScripts/generate_lessons.ps1")
    powershell.exe -NoProfile -ExecutionPolicy Bypass -File "$WIN_PATH"
else
    # --- LINUX / MAC (Run Bash) ---
    sh "$ROOT/BuildScripts/generate_lessons.sh"
fi