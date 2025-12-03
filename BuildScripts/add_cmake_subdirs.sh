#!/bin/bash

# 1. Define folders to ignore
IGNORE_LIST=(
    "." ".." ".git" ".vs" ".vscode" "build" "out" "external" "BuildScripts" "cmake-build-debug"
    "windows-msvc-debug" "windows-msvc-release" 
    "linux-gcc-debug" "linux-gcc-release"
)

OUTPUT_FILE="CMakeLists.txt"
echo "# Generated Category Registry" > "$OUTPUT_FILE"

count=0
# Loop over directories
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