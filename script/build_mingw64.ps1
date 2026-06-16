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

& $QmakePath -spec win32-g++ "CONFIG+=werror" "CodeEditorLite.pro" -o "$BuildDir\Makefile"

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

& $MakePath -f "Makefile.$BuildType" -j

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
        Write-Host "  Manually copying dependencies..." -ForegroundColor Cyan
    }
    
    # Copy QtCore DLL
    $QtCoreDllPath = "$QtBinPath\Qt5Core.dll"
    $QtCoreTargetPath = "$OutputDir\Qt5Core.dll"
    if (Test-Path $QtCoreDllPath) {
        Copy-Item -Path $QtCoreDllPath -Destination $QtCoreTargetPath -Force
        Write-Host "  Copied Qt5Core.dll" -ForegroundColor Green
    }
    
    # Copy QtGui DLL
    $QtGuiDllPath = "$QtBinPath\Qt5Gui.dll"
    $QtGuiTargetPath = "$OutputDir\Qt5Gui.dll"
    if (Test-Path $QtGuiDllPath) {
        Copy-Item -Path $QtGuiDllPath -Destination $QtGuiTargetPath -Force
        Write-Host "  Copied Qt5Gui.dll" -ForegroundColor Green
    }
    
    # Copy QtWidgets DLL
    $QtWidgetsDllPath = "$QtBinPath\Qt5Widgets.dll"
    $QtWidgetsTargetPath = "$OutputDir\Qt5Widgets.dll"
    if (Test-Path $QtWidgetsDllPath) {
        Copy-Item -Path $QtWidgetsDllPath -Destination $QtWidgetsTargetPath -Force
        Write-Host "  Copied Qt5Widgets.dll" -ForegroundColor Green
    }
    
    # Copy QtPrintSupport DLL
    $QtPrintSupportDllPath = "$QtBinPath\Qt5PrintSupport.dll"
    $QtPrintSupportTargetPath = "$OutputDir\Qt5PrintSupport.dll"
    if (Test-Path $QtPrintSupportDllPath) {
        Copy-Item -Path $QtPrintSupportDllPath -Destination $QtPrintSupportTargetPath -Force
        Write-Host "  Copied Qt5PrintSupport.dll" -ForegroundColor Green
    }
    
    # Copy QtSvg DLL (required for SVG icons)
    $QtSvgDllPath = "$QtBinPath\Qt5Svg.dll"
    $QtSvgTargetPath = "$OutputDir\Qt5Svg.dll"
    if (Test-Path $QtSvgDllPath) {
        Copy-Item -Path $QtSvgDllPath -Destination $QtSvgTargetPath -Force
        Write-Host "  Copied Qt5Svg.dll" -ForegroundColor Green
    }
    
    # Copy QtNetwork DLL
    $QtNetworkDllPath = "$QtBinPath\Qt5Network.dll"
    $QtNetworkTargetPath = "$OutputDir\Qt5Network.dll"
    if (Test-Path $QtNetworkDllPath) {
        Copy-Item -Path $QtNetworkDllPath -Destination $QtNetworkTargetPath -Force
        Write-Host "  Copied Qt5Network.dll" -ForegroundColor Green
    }
    
    # Copy QScintilla DLL (windeployqt cannot detect third-party libraries)
    $QScintillaDllPath = "$ProjectRoot\lib\qscintilla_mingw64\lib\qscintilla2_qt5.dll"
    $QScintillaTargetPath = "$OutputDir\qscintilla2_qt5.dll"
    
    if (Test-Path $QScintillaDllPath) {
        Copy-Item -Path $QScintillaDllPath -Destination $QScintillaTargetPath -Force
        Write-Host "  Copied QScintilla: qscintilla2_qt5.dll" -ForegroundColor Green
    } else {
        Write-Host "  Warning: QScintilla DLL not found at $QScintillaDllPath" -ForegroundColor Yellow
    }
    
    # Copy platform plugins (CRITICAL - windeployqt often misses this)
    $PlatformsSourcePath = "$QtDir\plugins\platforms"
    $PlatformsTargetPath = "$OutputDir\platforms"
    
    if (Test-Path $PlatformsSourcePath) {
        New-Item -ItemType Directory -Path $PlatformsTargetPath -Force | Out-Null
        Copy-Item -Path "$PlatformsSourcePath\qwindows.dll" -Destination $PlatformsTargetPath -Force
        Write-Host "  Copied platforms\qwindows.dll" -ForegroundColor Green
    } else {
        Write-Host "  Warning: Platform plugins not found at $PlatformsSourcePath" -ForegroundColor Yellow
    }
    
    # Copy imageformat plugins (CRITICAL for SVG icons)
    $ImageformatsSourcePath = "$QtDir\plugins\imageformats"
    $ImageformatsTargetPath = "$OutputDir\imageformats"
    
    if (Test-Path $ImageformatsSourcePath) {
        New-Item -ItemType Directory -Path $ImageformatsTargetPath -Force | Out-Null
        Copy-Item -Path "$ImageformatsSourcePath\qsvg.dll" -Destination $ImageformatsTargetPath -Force
        Write-Host "  Copied imageformats\qsvg.dll" -ForegroundColor Green
    } else {
        Write-Host "  Warning: Imageformat plugins not found at $ImageformatsSourcePath" -ForegroundColor Yellow
    }
    
    # Copy MinGW runtime DLLs
    $MinGWDlls = @("libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll")
    foreach ($dll in $MinGWDlls) {
        $SourcePath = "$MinGWPath\$dll"
        $TargetPath = "$OutputDir\$dll"
        if (Test-Path $SourcePath) {
            Copy-Item -Path $SourcePath -Destination $TargetPath -Force
            Write-Host "  Copied $dll" -ForegroundColor Green
        }
    }
} else {
    Write-Host "  Error: windeployqt not found at $WindeployqtPath" -ForegroundColor Red
    exit 1
}

# ========================================
# Copy External Resources (fonts and icons)
# ========================================
Write-Host "`nCopying external resources..." -ForegroundColor Cyan

$ResourcesDir = "$OutputDir\resources"
$IconSourceDir = "$ProjectRoot\src\resource\icon"
$FontSourceDir = "$ProjectRoot\src\resource\font"

# Copy icons
$IconTargetDir = "$ResourcesDir\icon"
if (Test-Path $IconSourceDir) {
    New-Item -ItemType Directory -Path $IconTargetDir -Force | Out-Null
    Copy-Item -Path "$IconSourceDir\**" -Destination $IconTargetDir -Recurse -Force
    Write-Host "  Copied icons to resources/icon" -ForegroundColor Green
} else {
    Write-Host "  Warning: Icon source directory not found at $IconSourceDir" -ForegroundColor Yellow
}

# Copy fonts
$FontTargetDir = "$ResourcesDir\font"
if (Test-Path $FontSourceDir) {
    New-Item -ItemType Directory -Path $FontTargetDir -Force | Out-Null
    Copy-Item -Path "$FontSourceDir\**" -Destination $FontTargetDir -Recurse -Force
    Write-Host "  Copied fonts to resources/font" -ForegroundColor Green
} else {
    Write-Host "  Warning: Font source directory not found at $FontSourceDir" -ForegroundColor Yellow
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