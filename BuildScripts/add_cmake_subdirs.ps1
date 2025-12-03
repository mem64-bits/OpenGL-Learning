# 1. Get the directory where the script is RUNNING
$CurrentDir = Get-Location

# ==============================================================================
# 🛡️ SAFETY CHECK: PREVENT RUNNING IN ROOT
# ==============================================================================
try {
    # Get the Git Root path
    $GitRoot = git rev-parse --show-toplevel 2>$null
    
    # Normalize paths to ensure comparison works (Git uses /, Windows uses \)
    $NormCurrent = $CurrentDir.Path.Replace('\', '/').TrimEnd('/')
    $NormRoot = $GitRoot.Replace('\', '/').TrimEnd('/')

    if ($NormCurrent -eq $NormRoot) {
        Write-Host "--------------------------------------------------" -ForegroundColor Red
        Write-Error "⛔ ABORTING: You are trying to run this in the Repository Root!"
        Write-Host "   This script generates a simple 'add_subdirectory' list." -ForegroundColor Yellow
        Write-Host "   Running it here would DELETE your main CMakeLists.txt logic." -ForegroundColor Yellow
        Write-Host "   Please 'cd' into 'Projects'folder first." -ForegroundColor Yellow
        Write-Host "--------------------------------------------------" -ForegroundColor Red
        exit 1
    }
} catch {
    # If git fails, we proceed with caution, but usually this is run in a git repo.
}
# ==============================================================================

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