# Quick prerequisite check before first CMake configure (Windows)
$ErrorActionPreference = "Continue"
$ok = $true

function Test-Item($label, $pass, $hint) {
    if ($pass) {
        Write-Host "[OK]   $label" -ForegroundColor Green
    } else {
        Write-Host "[FAIL] $label" -ForegroundColor Red
        if ($hint) { Write-Host "       $hint" -ForegroundColor Yellow }
        $ok = $false
    }
}

Write-Host "MyFirstPlugin environment check" -ForegroundColor Cyan
Write-Host ""

Test-Item "CMake on PATH" (Get-Command cmake -ErrorAction SilentlyContinue) "Install: winget install Kitware.CMake"

$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$hasVs = (Test-Path $vsWhere) -and (& $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath 2>$null)
Test-Item "Visual Studio C++ tools (MSVC)" $hasVs "Install VS 2022 workload: Desktop development with C++"

$git = Get-Command git -ErrorAction SilentlyContinue
Test-Item "Git on PATH (JUCE FetchContent)" $git "Install: winget install Git.Git"

Write-Host ""
if ($ok) {
    Write-Host "Ready. Run Build Plugin.bat option 3, or open this folder in Visual Studio." -ForegroundColor Green
    exit 0
}

Write-Host "Fix the items above, then run this script again." -ForegroundColor Red
exit 1
