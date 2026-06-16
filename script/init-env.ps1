# Qt5.14.2 Environment Initialization Script
# Usage: . .\script\init-env.ps1

# ========================================
# Configuration - Load user environment config
# ========================================
$EnvConfigPath = Join-Path $PSScriptRoot "env_config.ps1"

if (Test-Path $EnvConfigPath) {
    . $EnvConfigPath
    Write-Host "  Loaded environment config: $EnvConfigPath" -ForegroundColor Cyan
} else {
    Write-Host "  No env_config.ps1 found, using defaults." -ForegroundColor Yellow
    Write-Host "  Copy script\env_config.ps1.example to script\env_config.ps1 and edit it." -ForegroundColor Yellow
    # Default fallback paths
    $script:QtDir = "C:\Users\gzb17\Qt5\5.14.2\mingw73_64"
    $script:MinGWPath = "C:\Users\gzb17\Qt5\Tools\mingw730_64\bin"
}

$QtBasePath = Split-Path $script:QtDir -Parent
$QtMingwPath = $script:QtDir

# Add Qt MinGW bin and MinGW Tools to PATH
$env:PATH = "$MinGWPath;$QtMingwPath\bin;$env:PATH"

# Add libexec to PATH if exists
if (Test-Path "$QtMingwPath\libexec") {
    $env:PATH = "$QtMingwPath\libexec;$env:PATH"
}

# Set Qt environment variables
$env:QT_DIR = "$QtMingwPath"
$env:QT_BASE = "$QtBasePath"
$env:CMAKE_PREFIX_PATH = "$QtMingwPath;$env:CMAKE_PREFIX_PATH"
$env:QMAKE = "$QtMingwPath\bin\qmake.exe"

# Output status
Write-Host "Qt5.14.2 environment initialized" -ForegroundColor Green
Write-Host "QT_DIR: $env:QT_DIR"
Write-Host "QMAKE: $env:QMAKE"
Write-Host "Qt bin directory added to PATH"

# Verify qmake
if (Test-Path $env:QMAKE) {
    Write-Host "qmake found: $($env:QMAKE)" -ForegroundColor Green
} else {
    Write-Host "Warning: qmake not found" -ForegroundColor Yellow
}
