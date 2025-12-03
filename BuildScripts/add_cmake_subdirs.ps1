# 1. Get the directory where the script is RUNNING (now fixed by the alias)
$CurrentDir = Get-Location
$OutputFile = Join-Path $CurrentDir "CMakeLists.txt"

Write-Host "--------------------------------------------------" -ForegroundColor DarkGray
Write-Host "🔍 Scanning directory: $CurrentDir" -ForegroundColor Cyan

# 2. Define folders to IGNORE
$IgnoreList = @(
    "build", "out", ".git", ".vs", "external", "BuildScripts", "cmake-build-debug",
    "windows-msvc-debug", "windows-msvc-release", 
    "linux-gcc-debug", "linux-gcc-release"
)

# 3. Find directories, filter them, and sort them
$Folders = Get-ChildItem -Directory | 
           Where-Object { $_.Name -notin $IgnoreList } | 
           Sort-Object Name

# 4. Check if we found anything
if ($Folders.Count -eq 0) {
    Write-Warning "⚠️  No subdirectories found in this folder!"
    exit
}

# 5. Generate the content
$Output = @("# Generated Category Registry")
$Output += $Folders | ForEach-Object { 
    'add_subdirectory("{0}")' -f $_.Name 
}

# 6. Write to CMakeLists.txt
Set-Content -Path $OutputFile -Value $Output -Encoding UTF8

# 7. Confirm location to user
Write-Host "✅ Generated CMakeLists.txt with $($Folders.Count) entries." -ForegroundColor Green
Write-Host "📁 File saved at: $OutputFile" -ForegroundColor Yellow
Write-Host "--------------------------------------------------" -ForegroundColor DarkGray