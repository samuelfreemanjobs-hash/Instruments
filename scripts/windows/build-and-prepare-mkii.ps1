# Build NTS-1 mkII units via WSL, then list files and open Explorer for Kontrol Editor.
#
#   cd C:\dev\Instruments
#   .\scripts\windows\build-and-prepare-mkii.ps1
#
# Prepare only (skip WSL compile if release/ already has units):
#   .\scripts\windows\build-and-prepare-mkii.ps1 -PrepareOnly
#
# Execution policy:
#   powershell -ExecutionPolicy Bypass -File .\scripts\windows\build-and-prepare-mkii.ps1

[CmdletBinding()]
param(
    [switch]$PrepareOnly
)

$ErrorActionPreference = "Stop"

function Convert-ToWslPath {
    param([string]$WindowsPath)
    $full = [System.IO.Path]::GetFullPath($WindowsPath)
    if ($full -match '^([A-Za-z]):\\(.*)$') {
        $drive = $Matches[1].ToLowerInvariant()
        $tail = ($Matches[2] -replace '\\', '/').TrimEnd('/')
        return "/mnt/$drive/$tail"
    }
    throw "Cannot map path to WSL: $WindowsPath"
}

$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$PrepareScript = Join-Path $PSScriptRoot "prepare-mkii-test.ps1"
$MkiiDir = Join-Path $RepoRoot "release\nts1-custom-oscillators\mkII-nts1mkiiunit"

if (-not $PrepareOnly) {
    $wsl = Get-Command wsl.exe -ErrorAction SilentlyContinue
    if (-not $wsl) {
        Write-Host "WSL is not installed. Install WSL2 Ubuntu, or download the nts1-mkii-units artifact from GitHub Actions." -ForegroundColor Yellow
        Write-Host "See docs/NTS1_MKII_WINDOWS_TEST.md"
        exit 1
    }

    $wslRepo = Convert-ToWslPath -WindowsPath $RepoRoot
    Write-Host "WSL build in $wslRepo ..." -ForegroundColor Cyan
    & wsl.exe -e bash -lc "cd '$wslRepo' && bash ./scripts/windows/build-mkii-wsl.sh"
    if ($LASTEXITCODE -ne 0) {
        Write-Host "WSL build failed (exit $LASTEXITCODE)." -ForegroundColor Red
        exit $LASTEXITCODE
    }
} else {
  if (-not (Test-Path $MkiiDir)) {
        Write-Host "-PrepareOnly set but no release folder. Run without -PrepareOnly first." -ForegroundColor Yellow
        exit 1
    }
}

& $PrepareScript