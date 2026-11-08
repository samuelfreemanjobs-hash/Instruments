# CHORD ENGINE — Windows Release VST3 (Visual Studio 2022 x64)
# Usage from monorepo root:  .\DiskLordz-Audio\build-windows.ps1

$ErrorActionPreference = "Stop"
$Source = $PSScriptRoot
$Build = Join-Path $Source "Build\Windows"

Write-Host "Configure: $Build"
cmake -B $Build -S $Source -G "Visual Studio 17 2022" -A x64

Write-Host "Build: ChordEngine_VST3 (Release)"
cmake --build $Build --config Release --target ChordEngine_VST3

$Vst3 = Join-Path $Build "ChordEngine_artefacts\Release\VST3\CHORD ENGINE.vst3"
if (Test-Path $Vst3) {
    Write-Host "VST3: $Vst3"
} else {
    Write-Warning "Expected VST3 not found at $Vst3 — check build log."
}
