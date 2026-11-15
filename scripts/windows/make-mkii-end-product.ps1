# Build (WSL) + zip end product for Kontrol Editor.
#   .\scripts\windows\make-mkii-end-product.ps1

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
$Zip = Join-Path $RepoRoot "release\nts1-mkii-genre-oscillators.zip"

& (Join-Path $PSScriptRoot "build-and-prepare-mkii.ps1")
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$wslRepo = Convert-ToWslPath -WindowsPath $RepoRoot
& wsl.exe -e bash -lc "cd '$wslRepo' && bash ./tools/package-mkii-end-product.sh"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

if (Test-Path $Zip) {
    Write-Host ""
    Write-Host "END PRODUCT READY:" -ForegroundColor Green
    Write-Host "  $Zip"
    Write-Host "Unzip -> load .nts1mkiiunit in KORG KONTROL Editor (NTS-1 mkII -> Oscillator)."
    if ($env:OS -match "Windows") {
        Start-Process explorer.exe (Split-Path $Zip)
    }
} else {
    Write-Host "Zip not found: $Zip" -ForegroundColor Yellow
    exit 1
}