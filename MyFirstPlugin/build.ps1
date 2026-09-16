# Windows: configure + build MyFirstPlugin (no VS Code / no IDE extensions required)
$ErrorActionPreference = "Stop"
$Root = $PSScriptRoot
$BuildDir = Join-Path $Root "build"

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Error "cmake not found. Install CMake: https://cmake.org/download/ (or: winget install Kitware.CMake)"
}

Push-Location $Root

# Prefer CMake preset (matches Visual Studio); fall back to plain configure.
$usedPreset = $false
if (Test-Path (Join-Path $Root "CMakePresets.json")) {
    cmake --preset windows-vs-release 2>$null
    if ($LASTEXITCODE -eq 0) {
        $usedPreset = $true
        cmake --build --preset windows-vs-release
        if ($LASTEXITCODE -ne 0) { Pop-Location; exit $LASTEXITCODE }
    }
}

if (-not $usedPreset) {
    $Generator = @()
    if (Get-Command ninja -ErrorAction SilentlyContinue) {
        $Generator = @("-G", "Ninja")
    }
    cmake -B build -DCMAKE_BUILD_TYPE=Release @Generator
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    cmake --build build --config Release -j
}
$code = $LASTEXITCODE
Pop-Location
exit $code
