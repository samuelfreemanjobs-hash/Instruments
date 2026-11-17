# Build all logue SDK units in this repo (NTS-1 / XD / Prologue + mkII).
# Run from repo root in PowerShell: .\tools\windows\build-all-logue.ps1
param(
    [switch]$SkipBootstrap,
    [switch]$DockerMkii
)

$ErrorActionPreference = "Stop"
$Root = Resolve-Path (Join-Path $PSScriptRoot "..\..")
Set-Location $Root

function Invoke-Bash {
    param([string]$Command)
    $bash = Get-Command bash -ErrorAction SilentlyContinue
    if (-not $bash) {
        throw "bash not found. Install Git for Windows or use WSL2 (see docs/BUILD_WINDOWS_LOGUE.md)."
    }
    & $bash.Source -lc "cd '$($Root.Path.Replace('\','/'))' && $Command"
    if ($LASTEXITCODE -ne 0) { throw "Command failed: $Command" }
}

if (-not $SkipBootstrap) {
    Write-Host "==> Bootstrap logue-sdk (.deps/logue-sdk)"
    $env:MKII_SKIP_GCC = "1"
    Invoke-Bash "./tools/mkii-automate.sh bootstrap"
}

$logueSdk = Join-Path $Root ".deps/logue-sdk"
if (-not (Test-Path (Join-Path $logueSdk "platform/nutekt-digital"))) {
    throw "LOGUE_SDK not ready at $logueSdk — bootstrap failed?"
}

Write-Host "==> Build all v1 platforms + mkII"
Invoke-Bash "export LOGUE_SDK='$($logueSdk.Path.Replace('\','/'))' && ./tools/build-all-units.sh --all-platforms --mkii --package"

Write-Host ""
Write-Host "Done. Install packages: release/nts1-custom-oscillators/"
Write-Host "Build log: release/logue-build-status.tsv"
