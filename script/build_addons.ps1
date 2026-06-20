# build_addons.ps1 - Build per-language tree-sitter analyzer addons

$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $PSCommandPath)
$QtDir = "D:\Qt5\Qt5.14.2\5.14.2\mingw73_64"
$MingwDir = "D:\Qt5\Qt5.14.2\Tools\mingw730_64\bin"
$TreeSitterInc = "$ProjectRoot\lib\tree_sitter_mingw64\include"
$TreeSitterLib = "$ProjectRoot\lib\tree_sitter_mingw64\lib"
$ComponentInc = "$ProjectRoot\inc"
$GrammarSrc = "$ProjectRoot\lib\tree_sitter_mingw64\grammars"
$TemplateSrc = "$ProjectRoot\addons\template\src\addon_template.cpp"
$AddonsDir = "$ProjectRoot\addons"
$env:Path = "$MingwDir;$QtDir\bin;$env:PATH"

$languages = @(
    @{name="ts_c";       lexer="c";        grammar="ts_c.dll"}
    @{name="ts_cpp";     lexer="cpp";      grammar="ts_cpp.dll"}
    @{name="ts_python";  lexer="python";   grammar="ts_python.dll"}
    @{name="ts_javascript"; lexer="javascript"; grammar="ts_javascript.dll"}
    @{name="ts_bash";    lexer="bash";     grammar="ts_bash.dll"}
    @{name="ts_java";    lexer="java";     grammar="ts_java.dll"}
    @{name="ts_csharp";  lexer="csharp";   grammar="ts_c-sharp.dll"}
    @{name="ts_ruby";    lexer="ruby";     grammar="ts_ruby.dll"}
    @{name="ts_json";    lexer="json";     grammar="ts_json.dll"}
    @{name="ts_verilog"; lexer="verilog";  grammar="ts_verilog.dll"}
)

Write-Output "=== Building per-language addons ==="

# Clean old
Get-ChildItem $AddonsDir -Directory | Where-Object { $_.Name -ne "template" } | ForEach-Object {
    Remove-Item -Recurse -Force $_.FullName
}

foreach ($lang in $languages) {
    $name = $lang.name; $lexer = $lang.lexer; $grammar = $lang.grammar
    $addonDir = "$AddonsDir\$name"
    Write-Output "  $name ($lexer)..."

    New-Item -ItemType Directory -Path "$addonDir\grammars" -Force | Out-Null

    # addon.xml
    $xml = @"
<?xml version="1.0" encoding="UTF-8"?>
<addon>
    <name>$name</name>
    <vendor>CodeEditorLite Team</vendor>
    <displayName>$lexer Analyzer</displayName>
    <version>1.0.0</version>
    <description>Tree-sitter analysis for $lexer</description>
    <files>
        <file platform="win">$name.dll</file>
        <file platform="mac">lib$name.dylib</file>
        <file platform="linux">lib$name.so</file>
    </files>
    <capabilities>
        <capability>symbol_outline</capability>
        <capability>segment_info</capability>
    </capabilities>
</addon>
"@
    Set-Content -Path "$addonDir\addon.xml" -Value $xml -Encoding UTF8

    if (Test-Path "$GrammarSrc\$grammar") {
        Copy-Item "$GrammarSrc\$grammar" "$addonDir\grammars\" -Force
    }

    $allFlags = @(
        "-std=c++11"; "-fno-keep-inline-dllexport"; "-O2"; "-shared"
        "-DLANG_NAME=$lexer"
        "-o", "$addonDir\$name.dll"
        "-I$TreeSitterInc"; "-I$ComponentInc"
        "-I$QtDir\include"; "-I$QtDir\include\QtCore"; "-I$QtDir\include\QtWidgets"
        "-L$TreeSitterLib"; "-llibtree-sitter"
        "-L$QtDir\lib"; "-lQt5Core"; "-lQt5Widgets"
    )

    $result = Invoke-Expression "g++ `"$TemplateSrc`" $allFlags 2>&1"
    if ($LASTEXITCODE -eq 0 -and (Test-Path "$addonDir\$name.dll")) {
        Write-Output "    OK"
    } else {
        Write-Output "    FAILED:"
        $result | ForEach-Object { Write-Output "      $_" }
    }
}

# Copy libtree-sitter.dll
$tsBin = "$ProjectRoot\lib\tree_sitter_mingw64\bin\libtree-sitter.dll"
if (Test-Path $tsBin) {
    Get-ChildItem $AddonsDir -Directory | Where-Object { $_.Name -ne "template" } | ForEach-Object {
        if (Test-Path "$($_.FullName)\$($_.Name).dll") {
            Copy-Item $tsBin $_.FullName -Force
        }
    }
}

Write-Output "=== Done ==="
Get-ChildItem $AddonsDir -Directory | Where-Object { $_.Name -ne "template" } | ForEach-Object {
    $ok = if (Test-Path "$($_.FullName)\$($_.Name).dll") { "OK" } else { "MISSING" }
    Write-Output "  $($_.Name): $ok"
}
