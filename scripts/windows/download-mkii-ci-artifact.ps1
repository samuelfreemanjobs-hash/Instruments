# Download the latest successful nts1-mkii-units artifact from GitHub Actions (no WSL compile).
# Requires: gh auth login (GitHub CLI)
#
#   cd C:\dev\Instruments
#   .\scripts\windows\download-mkii-ci-artifact.ps1
#   .\scripts\windows\prepare-mkii-test.ps1

$ErrorActionPreference = "Stop"

$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$ReleaseRoot = Join-Path $RepoRoot "release\nts1-custom-oscillators"
$ArtifactName = "nts1-mkii-units"
$WorkflowFile = "nts1-mkii-build.yml"

$gh = Get-Command gh -ErrorAction SilentlyContinue
if (-not $gh) {
    Write-Host "Install GitHub CLI and run: gh auth login" -ForegroundColor Yellow
    Write-Host "https://cli.github.com/"
    exit 1
}

Push-Location $RepoRoot
try {
    $runJson = gh run list --workflow $WorkflowFile --limit 20 --json databaseId,conclusion,status,headBranch `
        | ConvertFrom-Json
    $success = $runJson | Where-Object { $_.conclusion -eq "success" } | Select-Object -First 1
    if (-not $success) {
        Write-Host "No successful run for workflow $WorkflowFile yet." -ForegroundColor Yellow
        Write-Host "Open Actions on GitHub and run workflow 'NTS-1 mkII oscillators' (workflow_dispatch)."
        exit 1
    }

    $runId = $success.databaseId
    Write-Host "Downloading artifact from run $runId (branch hint: $($success.headBranch)) ..." -ForegroundColor Cyan

    $staging = Join-Path $env:TEMP "nts1-mkii-download-$runId"
    if (Test-Path $staging) {
        Remove-Item -Recurse -Force $staging
    }
    New-Item -ItemType Directory -Path $staging | Out-Null

    gh run download $runId --name $ArtifactName --dir $staging
    if ($LASTEXITCODE -ne 0) {
        Write-Host "gh run download failed." -ForegroundColor Red
        exit $LASTEXITCODE
    }

    New-Item -ItemType Directory -Force -Path $ReleaseRoot | Out-Null
    Copy-Item -Path (Join-Path $staging "*") -Destination $ReleaseRoot -Recurse -Force

    Write-Host "Installed under $ReleaseRoot" -ForegroundColor Green
    Write-Host "Next: .\scripts\windows\prepare-mkii-test.ps1"
} finally {
    Pop-Location
}