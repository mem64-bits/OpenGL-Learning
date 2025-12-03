#!/bin/bash

# 1. Get Root
ROOT=$(git rev-parse --show-toplevel)
echo "🌱 Generating Lesson Configs in: $ROOT"

# 2. Find all main.cpp files
#    We use 'grep -v' to exclude libraries and build output folders
find "$ROOT" -type f -name "main.cpp" | \
grep -v "/external/" | \
grep -v "/build/" | \
grep -v "/out/" | \
grep -v "/cmake-build-" | \
grep -v "/windows-msvc-" | \
grep -v "/linux-gcc-" | \
while read -r file; do

    # Path logic: file is .../LessonName/src/main.cpp
    src_dir=$(dirname "$file")
    lesson_dir=$(dirname "$src_dir")
    folder_name=$(basename "$lesson_dir")

    # Sanitize Target Name: 
    # Replace non-alphanumeric characters with underscore
    target_name=$(echo "$folder_name" | sed 's/[^a-zA-Z0-9_]/_/g')

    cmake_file="$lesson_dir/CMakeLists.txt"

    # Only create if it doesn't exist (prevents overwriting manual changes)
    if [ ! -f "$cmake_file" ]; then
        echo "create_lesson($target_name)" > "$cmake_file"
        echo "   + Created: $folder_name -> ($target_name)"
    fi
done

echo "✅ All lessons configured."