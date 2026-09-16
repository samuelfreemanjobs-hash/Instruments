# Simple menu interface for CMake (double-click Build Plugin.bat or run in PowerShell)
$ErrorActionPreference = "Stop"
$Root = $PSScriptRoot
Set-Location $Root

function Require-CMake {
    if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
        Write-Host "CMake not found. Install from https://cmake.org/download/ or: winget install Kitware.CMake" -ForegroundColor Red
        exit 1
    }
}

function Get-Vst3Path {
    $candidates = @(
        (Join-Path $Root "build\MyFirstPlugin_artefacts\Release\VST3\MyFirstPlugin.vst3"),
        (Join-Path $Root "build-ninja\MyFirstPlugin_artefacts\Release\VST3\MyFirstPlugin.vst3")
    )
    foreach ($p in $candidates) {
        if (Test-Path $p) { return $p }
    }
    return $null
}

Require-CMake

do {
    Write-Host ""
    Write-Host "========== MyFirstPlugin (CMake) ==========" -ForegroundColor Cyan
    Write-Host "  1  Configure (first time / after CMakeLists changes)"
    Write-Host "  2  Build Release VST3"
    Write-Host "  3  Configure + Build (all-in-one)"
    Write-Host "  4  Open VST3 folder in File Explorer"
    Write-Host "  5  Open this folder in Visual Studio (CMake)"
    Write-Host "  Q  Quit"
    Write-Host ""
    $choice = Read-Host "Choose"

    switch ($choice.ToUpper()) {
        "1" {
            Write-Host "Configuring with preset windows-vs-release ..." -ForegroundColor Yellow
            cmake --preset windows-vs-release
        }
        "2" {
            Write-Host "Building ..." -ForegroundColor Yellow
            cmake --build --preset windows-vs-release
            $vst = Get-Vst3Path
            if ($vst) { Write-Host "Built: $vst" -ForegroundColor Green }
        }
        "3" {
            & (Join-Path $Root "build.ps1")
            $vst = Get-Vst3Path
            if ($vst) { Write-Host "Built: $vst" -ForegroundColor Green }
        }
        "4" {
            $vst = Get-Vst3Path
            if (-not $vst) {
                Write-Host "No VST3 yet. Run option 2 or 3 first." -ForegroundColor Red
            } else {
                explorer.exe "/select,$vst"
            }
        }
        "5" {
            $devenv = @(
                "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe",
                "${env:ProgramFiles}\Microsoft Visual Studio\2022\Professional\Common7\IDE\devenv.exe",
                "${env:ProgramFiles}\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\devenv.exe"
            ) | Where-Object { Test-Path $_ } | Select-Object -First 1
            if ($devenv) {
                Start-Process $devenv -ArgumentList $Root
            } else {
                Write-Host "Visual Studio 2022 not found. Use: File -> Open -> Folder -> MyFirstPlugin" -ForegroundColor Yellow
            }
        }
        "Q" { break }
        default { Write-Host "Unknown option." -ForegroundColor Red }
    }
} while ($choice.ToUpper() -ne "Q")
