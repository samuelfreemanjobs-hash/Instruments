$ErrorActionPreference = "Stop"
$EngineRoot = Split-Path -Parent $PSScriptRoot
$env:PATH = "$(Join-Path $env:USERPROFILE '.cargo\bin');$env:PATH"
$Cargo = Join-Path $env:USERPROFILE ".cargo\bin\cargo.exe"
if (-not (Test-Path $Cargo)) { throw "Rust/Cargo is required; run scripts/setup.ps1 first" }
& (Join-Path $PSScriptRoot "build-engine.ps1")
if ($LASTEXITCODE -ne 0) { throw "Engine sidecar build failed" }
$PythonCommand = Get-Command python3.12, python -ErrorAction SilentlyContinue | Select-Object -First 1
if (-not $PythonCommand) { throw "Python 3.12+ is required" }
Push-Location $EngineRoot
try {
    & $PythonCommand.Source -m pytest
    if ($LASTEXITCODE -ne 0) { throw "Songwriting engine tests failed" }
} finally {
    Pop-Location
}
$DesktopRoot = Join-Path $EngineRoot "apps\desktop"
Push-Location $DesktopRoot
try {
    npm.cmd run build
    if ($LASTEXITCODE -ne 0) { throw "Desktop frontend build failed" }
} finally {
    Pop-Location
}
Push-Location (Join-Path $DesktopRoot "src-tauri")
try {
    & $Cargo check
    if ($LASTEXITCODE -ne 0) { throw "Tauri Rust compile check failed" }
} finally {
    Pop-Location
}
Write-Host "Engine tests, frontend build, and Tauri compile check passed."