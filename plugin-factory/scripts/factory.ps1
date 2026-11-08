# VST Plugin Factory OS — Windows entry (build all registered plugins).
$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
Set-Location $Root

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Error "cmake not found on PATH."
}

cmake --preset factory-windows-vs
cmake --build --preset factory-windows-vs

Write-Host ""
Write-Host "Artifacts: $Root\build\*_artefacts\Release\VST3\"
