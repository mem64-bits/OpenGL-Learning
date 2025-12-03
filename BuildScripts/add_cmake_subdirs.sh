#!/bin/bash

# --- SAFETY CHECK ---
ROOT_DIR=$(git rev-parse --show-toplevel)
CURRENT_DIR=$(pwd)

if [[ "$ROOT_DIR" == "$CURRENT_DIR" ]]; then
    echo "❌ ERROR: You are in the Repository Root!"
    echo "   Running this here would delete your main CMakeLists.txt."
    echo "   Please cd into a Projects subfolder."
    exit 1
fi
# --------------------

# 1. Define folders to ignore
IGNORE_LIST=(
    "." ".." ".git" ".vs" ".vscode" "build" "out" "external" "BuildScripts" "cmake-build-debug"
    "windows-msvc-debug" "windows-msvc-release" 
    "linux-gcc-debug" "linux-gcc-release"
)

OUTPUT_FILE="CMakeLists.txt"
echo "# Generated Category Registry" > "$OUTPUT_FILE"

count=0
for dir in */ ; do
    [ -d "$dir" ] || continue
    dirname=${dir%/}

    should_skip=false
    for ignore in "${IGNORE_LIST[@]}"; do
        if [[ "$dirname" == "$ignore" ]]; then
            should_skip=true
            break
        fi
    done

    if [[ "$should_skip" == "false" ]]; then
        echo "add_subdirectory(\"$dirname\")" >> "$OUTPUT_FILE"
        ((count++))
    fi
done

echo "✅ Generated CMakeLists.txt with $count entries."
echo "📁 File saved at: $(pwd)/$OUTPUT_FILE"