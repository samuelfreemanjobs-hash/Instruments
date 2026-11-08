$ErrorActionPreference = "Stop"
$EngineRoot = Split-Path -Parent $PSScriptRoot
$DesktopRoot = Join-Path $EngineRoot "apps\desktop"
$PythonCommand = Get-Command python3.12, python -ErrorAction SilentlyContinue | Select-Object -First 1
if (-not $PythonCommand) { throw "Python 3.12+ is required to build the engine sidecar" }

Push-Location $EngineRoot
try {
    $SidecarRoot = Join-Path $EngineRoot ".tmp\pyinstaller"
    $SidecarDist = Join-Path $SidecarRoot "dist"
    $SidecarWork = Join-Path $SidecarRoot "work"
    $SidecarSpec = Join-Path $SidecarRoot "spec"
    & $PythonCommand.Source -m PyInstaller --noconfirm --onefile --name songwriting-engine --collect-submodules mido --distpath $SidecarDist --workpath $SidecarWork --specpath $SidecarSpec .\songwriting\sidecar.py
    if ($LASTEXITCODE -ne 0) { throw "PyInstaller engine sidecar build failed" }
    $ResourceDir = Join-Path $DesktopRoot "src-tauri\resources"
    New-Item -ItemType Directory -Path $ResourceDir -Force | Out-Null
    Copy-Item (Join-Path $SidecarDist "songwriting-engine.exe") (Join-Path $ResourceDir "songwriting-engine.exe") -Force
    Write-Host "Engine sidecar ready: $ResourceDir\songwriting-engine.exe"
} finally {
    Pop-Location
}