# Windows: configure + build MyFirstPlugin (no VS Code / no IDE extensions required)
$ErrorActionPreference = "Stop"
$Root = $PSScriptRoot
$BuildDir = Join-Path $Root "build"

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Error "cmake not found. Install CMake: https://cmake.org/download/ (or: winget install Kitware.CMake)"
}

$Generator = @()
if (Get-Command ninja -ErrorAction SilentlyContinue) {
    $Generator = @("-G", "Ninja")
}

Push-Location $Root
cmake -B build -DCMAKE_BUILD_TYPE=Release @Generator
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
cmake --build build --config Release -j
$code = $LASTEXITCODE
Pop-Location
exit $code
