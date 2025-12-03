# BuildScripts/generate_lessons.ps1

$Root = git rev-parse --show-toplevel
Write-Host "🌱 Generating Lesson Configs in: $Root" -ForegroundColor Cyan

# 1. Define Ignore Pattern
#    Using regex for cleaner filtering of multiple build folders
$IgnorePattern = "external|build|out|cmake-build|windows-msvc-|linux-gcc-"

# 2. Find every 'main.cpp'
$MainFiles = Get-ChildItem -Path $Root -Recurse -Filter "main.cpp" | 
             Where-Object { $_.FullName -notmatch $IgnorePattern }

foreach ($file in $MainFiles) {
    $LessonDir = $file.Directory.Parent.FullName 
    $FolderName = Split-Path $LessonDir -Leaf
    
    # Sanitize Target Name
    $TargetName = $FolderName -replace "[^a-zA-Z0-9_]", "_"
    
    $CMakeFile = Join-Path $LessonDir "CMakeLists.txt"

    if (-not (Test-Path $CMakeFile)) {
        $Content = "create_lesson($TargetName)"
        Set-Content -Path $CMakeFile -Value $Content -Encoding UTF8
        Write-Host "   + Created: $FolderName -> ($TargetName)" -ForegroundColor Green
    }
}

Write-Host "✅ All lessons configured." -ForegroundColor Cyan