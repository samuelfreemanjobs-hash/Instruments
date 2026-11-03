$ErrorActionPreference = "Stop"
$DesktopRoot = Join-Path (Split-Path -Parent $PSScriptRoot) "apps\desktop"
$Cargo = Join-Path $env:USERPROFILE ".cargo\bin\cargo.exe"
if (-not (Test-Path $Cargo)) { throw "Rust/Cargo is required; run scripts/setup.ps1 first" }
$env:PATH = "$(Join-Path $env:USERPROFILE '.cargo\bin');$env:PATH"
& (Join-Path $PSScriptRoot "build-engine.ps1")
if ($LASTEXITCODE -ne 0) { throw "Engine sidecar build failed" }
Push-Location $DesktopRoot
try {
    npm.cmd run desktop:build
    if ($LASTEXITCODE -ne 0) { throw "Windows desktop packaging failed" }
} finally {
    Pop-Location
}