# BuildScripts/generate_lessons.ps1

# 1. Get the Repo Root (so it works no matter where you run it from)
$Root = git rev-parse --show-toplevel
Write-Host "🌱 Generating Lesson Configs in: $Root" -ForegroundColor Cyan

# 2. Find every 'main.cpp' in the project
#    We strictly ignore 'external' (libraries) and build folders
$MainFiles = Get-ChildItem -Path $Root -Recurse -Filter "main.cpp" | 
             Where-Object { 
                $_.FullName -notmatch "external" -and 
                $_.FullName -notmatch "build" -and 
                $_.FullName -notmatch "out" -and
                $_.FullName -notmatch "cmake-build"
             }

foreach ($file in $MainFiles) {
    # The file path is like: .../Category/LessonName/src/main.cpp
    
    # Get the "Lesson" directory (Parent of 'src')
    $LessonDir = $file.Directory.Parent.FullName 
    
    # Get the Folder Name (e.g., "GLFW Window")
    $FolderName = Split-Path $LessonDir -Leaf
    
    # Create a "Safe" Target Name for CMake (e.g., "GLFW_Window")
    # CMake targets cannot contain spaces
    $TargetName = $FolderName -replace " ", "_"
    
    # Define where the CMakeLists.txt goes
    $CMakeFile = Join-Path $LessonDir "CMakeLists.txt"

    # 3. Create the file content
    # This calls the function defined in your Root CMakeLists.txt
    $Content = "create_lesson($TargetName)"

    # 4. Write the file
    # We overwrite it to ensure the target name always matches the folder name
    Set-Content -Path $CMakeFile -Value $Content -Encoding UTF8
    
    Write-Host "   + Configured: $FolderName -> ($TargetName)" -ForegroundColor Green
}

Write-Host "✅ All lessons configured." -ForegroundColor Cyan