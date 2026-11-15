# Print gh auth + workflow runs (debug "No successful run").
param(
    [string]$Repo = "samuelfreemanjobs-hash/Instruments"
)

$ErrorActionPreference = "Continue"
Write-Host "Repo root: $(Resolve-Path (Join-Path $PSScriptRoot '..\..'))"
Write-Host "gh: $(Get-Command gh -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Source)"
Write-Host ""
& gh --version
Write-Host ""
& gh auth status
Write-Host ""
Write-Host "Workflow file: nts1-mkii-build.yml" -ForegroundColor Cyan
& gh run list -R $Repo --workflow nts1-mkii-build.yml --limit 10
Write-Host ""
Write-Host "Workflow name: NTS-1 mkII oscillators" -ForegroundColor Cyan
& gh run list -R $Repo --workflow "NTS-1 mkII oscillators" --limit 5