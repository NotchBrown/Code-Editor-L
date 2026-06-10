<#
.SYNOPSIS
Build script for CodeEditorLite using MinGW64.

.DESCRIPTION
This script builds CodeEditorLite using Qt5.14.2 and MinGW64.
It supports Debug/Release modes and optional timestamped output directories.

.PARAMETER Debug
Build in debug mode instead of release mode.

.PARAMETER AddTimestamp
Add date/time suffix to the build output directory.

.EXAMPLE
.\build_mingw64.ps1
Builds in release mode with standard output directory.

.EXAMPLE
.\build_mingw64.ps1 -Debug
Builds in debug mode.

.EXAMPLE
.\build_mingw64.ps1 -AddTimestamp
Builds with timestamped output directory.
#>

param(
    [switch]$Debug,
    [switch]$AddTimestamp
)

# ========================================
# Configuration - Update these paths as needed
# ========================================
$QtDir = "D:\Qt5\Qt5.14.2\5.14.2\mingw73_64"
$MinGWPath = "D:\Qt5\Qt5.14.2\Tools\mingw730_64\bin"
$ProjectRoot = Split-Path -Parent $PSScriptRoot

# ========================================
# Initialize Environment
# ========================================
Write-Host "`nInitializing Qt environment..." -ForegroundColor Cyan
$QtBinPath = "$QtDir\bin"

if (-not (Test-Path $QtBinPath)) {
    Write-Host "Error: Qt not found at $QtBinPath" -ForegroundColor Red
    exit 1
}

if (-not (Test-Path $MinGWPath)) {
    Write-Host "Error: MinGW not found at $MinGWPath" -ForegroundColor Red
    exit 1
}

$env:PATH = "$MinGWPath;$QtBinPath;$env:PATH"

Write-Host "  Qt Dir: $QtDir"
Write-Host "  MinGW: $MinGWPath"
Write-Host "  PATH updated with Qt and MinGW"

# ========================================
# Set Build Type and Directories
# ========================================
$BuildType = if ($Debug) { "debug" } else { "release" }
$BuildDirBase = "$ProjectRoot\dist\build_$BuildType"

if ($AddTimestamp) {
    $Timestamp = Get-Date -Format "yyyyMMdd_HHmmss"
    $BuildDir = "$BuildDirBase" + "_$Timestamp"
} else {
    $BuildDir = $BuildDirBase
}

$OutputDir = "$BuildDir\bin"

Write-Host "`nCreating build directories..." -ForegroundColor Cyan
Write-Host "  Build Type: $BuildType"
Write-Host "  Build Directory: $BuildDir"

New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null

# ========================================
# Run qmake
# ========================================
Set-Location $ProjectRoot
Write-Host "`nRunning qmake..." -ForegroundColor Cyan

$QmakePath = "$QtBinPath\qmake.exe"
if (-not (Test-Path $QmakePath)) {
    Write-Host "Error: qmake not found at $QmakePath" -ForegroundColor Red
    exit 1
}

& $QmakePath -spec win32-g++ "CodeEditorLite.pro" -o "$BuildDir\Makefile"

if ($LASTEXITCODE -ne 0) {
    Write-Host "Error: qmake failed" -ForegroundColor Red
    exit 1
}

# ========================================
# Run mingw32-make
# ========================================
Write-Host "`nRunning mingw32-make (MinGW64)..." -ForegroundColor Cyan

Set-Location $BuildDir
$MakePath = "$MinGWPath\mingw32-make.exe"

if (-not (Test-Path $MakePath)) {
    Write-Host "Error: mingw32-make not found at $MakePath" -ForegroundColor Red
    exit 1
}

& $MakePath -f "Makefile.$BuildType"

if ($LASTEXITCODE -ne 0) {
    Write-Host "Error: mingw32-make failed" -ForegroundColor Red
    exit 1
}

# ========================================
# Copy Dependencies using windeployqt
# ========================================
Set-Location $ProjectRoot
Write-Host "`nRunning windeployqt..." -ForegroundColor Cyan

$WindeployqtPath = "$QtBinPath\windeployqt.exe"
$ExePath = "$OutputDir\CodeEditorLite.exe"

if (Test-Path $WindeployqtPath) {
    $BuildArg = if ($BuildType -eq "release") { "--release" } else { "--debug" }
    
    Write-Host "  Command: $WindeployqtPath $BuildArg `"$ExePath`""
    
    $env:PATH = "$QtBinPath;$env:PATH"
    & $WindeployqtPath $BuildArg $ExePath
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  windeployqt succeeded" -ForegroundColor Green
    } else {
        Write-Host "  Note: windeployqt exited with code $LASTEXITCODE" -ForegroundColor Cyan
    }
    
    # Copy QScintilla DLL (windeployqt cannot detect third-party libraries)
    $QScintillaDllPath = "$ProjectRoot\lib\QScintilla\src\release\qscintilla2_qt5.dll"
    $QScintillaTargetPath = "$OutputDir\qscintilla2_qt5.dll"
    
    if (Test-Path $QScintillaDllPath) {
        Copy-Item -Path $QScintillaDllPath -Destination $QScintillaTargetPath -Force
        Write-Host "  Copied QScintilla: qscintilla2_qt5.dll" -ForegroundColor Green
    } else {
        Write-Host "  Warning: QScintilla DLL not found at $QScintillaDllPath" -ForegroundColor Yellow
    }
    
    # Copy QtNetwork DLL (sometimes windeployqt misses it)
    $QtNetworkDllPath = "$QtBinPath\Qt5Network.dll"
    $QtNetworkTargetPath = "$OutputDir\Qt5Network.dll"
    
    if (Test-Path $QtNetworkDllPath) {
        Copy-Item -Path $QtNetworkDllPath -Destination $QtNetworkTargetPath -Force
        Write-Host "  Copied Qt5Network.dll" -ForegroundColor Green
    } else {
        Write-Host "  Warning: Qt5Network.dll not found at $QtNetworkDllPath" -ForegroundColor Yellow
    }
} else {
    Write-Host "  Error: windeployqt not found at $WindeployqtPath" -ForegroundColor Red
    exit 1
}

# ========================================
# Clean Intermediate Files
# ========================================
Write-Host "`nCleaning intermediate files..." -ForegroundColor Cyan

$IntermediateDirs = @(
    "$BuildDir\build",
    "$BuildDir\.qmake.stash",
    "$BuildDir\Makefile",
    "$BuildDir\Makefile.release",
    "$BuildDir\Makefile.Debug",
    "$BuildDir\release",
    "$BuildDir\debug"
)

foreach ($item in $IntermediateDirs) {
    if (Test-Path $item) {
        Remove-Item -Path $item -Recurse -Force -ErrorAction SilentlyContinue
        Write-Host "  Removed: $item"
    }
}

# ========================================
# Verify Build
# ========================================
Write-Host "`nVerifying build..." -ForegroundColor Cyan

if (Test-Path $ExePath) {
    Write-Host "Build succeeded!" -ForegroundColor Green
    Write-Host "Output: $ExePath`n"
    
    Write-Host "Dist directory structure:`n"
    Get-ChildItem -Path $BuildDir -Recurse | Select-Object FullName | Format-Table -AutoSize
    
    exit 0
} else {
    Write-Host "Error: Build failed - executable not found" -ForegroundColor Red
    exit 1
}