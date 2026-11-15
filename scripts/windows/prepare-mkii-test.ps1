# Open the packaged mkII oscillator folder and list .nts1mkiiunit files for Kontrol Editor.
# Run from repo root: .\scripts\windows\prepare-mkii-test.ps1

$ErrorActionPreference = "Stop"
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$MkiiDir = Join-Path $RepoRoot "release\nts1-custom-oscillators\mkII-nts1mkiiunit"

if (-not (Test-Path $MkiiDir)) {
    Write-Host "No packaged mkII units at:" -ForegroundColor Yellow
    Write-Host "  $MkiiDir"
    Write-Host ""
    Write-Host "Build in WSL2, then run ./tools/package-nts1.sh"
    Write-Host "Or download the nts1-mkii-units artifact from GitHub Actions."
    Write-Host "See docs/NTS1_MKII_WINDOWS_TEST.md"
    exit 1
}

$units = Get-ChildItem -Path $MkiiDir -Filter "*.nts1mkiiunit" | Sort-Object Name
if ($units.Count -eq 0) {
    Write-Host "Folder exists but no .nts1mkiiunit files. Re-run package or CI build." -ForegroundColor Yellow
    exit 1
}

Write-Host "NTS-1 mkII units ready ($($units.Count) files):" -ForegroundColor Green
foreach ($u in $units) {
    $kb = [math]::Round($u.Length / 1KB, 1)
    Write-Host ("  {0,-42} {1,6} KB" -f $u.Name, $kb)
}

Write-Host ""
Write-Host "KORG KONTROL Editor -> NTS-1 mkII -> Oscillator -> Load one of the files above."
Write-Host "Docs: docs/NTS1_MKII_WINDOWS_TEST.md"
Write-Host ""

if ($env:OS -match "Windows") {
    Start-Process explorer.exe $MkiiDir
}