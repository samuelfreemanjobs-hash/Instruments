# Download the latest successful nts1-mkii-units artifact from GitHub Actions (no WSL compile).
# Requires: gh auth login (GitHub CLI)
#
#   cd C:\dev\Instruments
#   .\scripts\windows\download-mkii-ci-artifact.ps1
#   .\scripts\windows\prepare-mkii-test.ps1

param(
    [string]$Repo = "samuelfreemanjobs-hash/Instruments",
    [string]$Branch = "main"
)

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

gh auth status 2>&1 | Out-Null
if ($LASTEXITCODE -ne 0) {
    Write-Host "GitHub CLI not authenticated. Run: gh auth login" -ForegroundColor Yellow
    exit 1
}

Push-Location $RepoRoot
try {
    $raw = gh run list -R $Repo --workflow $WorkflowFile --limit 30 --json databaseId,conclusion,status,headBranch,url
    if ($LASTEXITCODE -ne 0) {
        Write-Host "gh run list failed for $Repo" -ForegroundColor Red
        exit $LASTEXITCODE
    }
    if ([string]::IsNullOrWhiteSpace($raw) -or $raw.Trim() -eq "[]") {
        Write-Host "No workflow runs returned for $Repo / $WorkflowFile." -ForegroundColor Yellow
        Write-Host "Trigger: gh workflow run $WorkflowFile -R $Repo --ref main"
        exit 1
    }

    $runJson = $raw | ConvertFrom-Json
    if ($runJson -isnot [array]) {
        $runJson = @($runJson)
    }

    $success = $runJson | Where-Object { $_.conclusion -eq "success" -and $_.headBranch -eq $Branch } | Select-Object -First 1
    if (-not $success) {
        $success = $runJson | Where-Object { $_.conclusion -eq "success" } | Select-Object -First 1
    }
    if (-not $success) {
        Write-Host "No successful run for workflow $WorkflowFile on $Repo yet." -ForegroundColor Yellow
        Write-Host "Trigger: gh workflow run $WorkflowFile -R $Repo --ref main"
        Write-Host "Or: GitHub -> Actions -> NTS-1 mkII oscillators -> Run workflow"
        exit 1
    }

    $runId = $success.databaseId
    Write-Host "Downloading artifact from run $runId (branch: $($success.headBranch)) ..." -ForegroundColor Cyan
    if ($success.url) {
        Write-Host "Run: $($success.url)"
    }

    $staging = Join-Path $env:TEMP "nts1-mkii-download-$runId"
    if (Test-Path $staging) {
        Remove-Item -Recurse -Force $staging
    }
    New-Item -ItemType Directory -Path $staging | Out-Null

    gh run download $runId -R $Repo --name $ArtifactName --dir $staging
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