# CHORD ENGINE — Windows Release VST3 (Visual Studio 2022 x64)
# Usage from monorepo root:  .\DiskLordz-Audio\build-windows.ps1

$ErrorActionPreference = "Stop"
$Source = $PSScriptRoot
$Build = Join-Path $Source "Build\Windows"

Write-Host "Configure: $Build"
cmake -B $Build -S $Source -G "Visual Studio 17 2022" -A x64

Write-Host "Build: ChordEngine_VST3 (Release)"
cmake --build $Build --config Release --target ChordEngine_VST3

$Vst3 = Get-ChildItem -Path $Build -Recurse -Filter "CHORD ENGINE.vst3" -Directory -ErrorAction SilentlyContinue | Select-Object -First 1
if ($Vst3) {
    Write-Host "VST3: $($Vst3.FullName)"
} else {
    Write-Warning "CHORD ENGINE.vst3 bundle not found under $Build — check build log."
}
