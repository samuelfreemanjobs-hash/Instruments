# Download nts1-mkii-units from GitHub Actions (no WSL compile).
# Requires: gh auth login — same GitHub user that can read this repo.
#
#   .\scripts\windows\download-mkii-ci-artifact.ps1
#   .\scripts\windows\diagnose-mkii-download.ps1   # if this script fails

param(
    [string]$Repo = "",
    [string]$Branch = "main",
    [long]$RunId = 0
)

$ErrorActionPreference = "Stop"

$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$ReleaseRoot = Join-Path $RepoRoot "release\nts1-custom-oscillators"
$ArtifactName = "nts1-mkii-units"

function Get-GhRepoFromGit {
    param([string]$Root)
    try {
        $url = git -C $Root remote get-url origin 2>$null
        if ($url -match 'github\.com[:/](.+)/(.+?)(\.git)?$') {
            return "$($Matches[1])/$($Matches[2])"
        }
    } catch {}
    return "samuelfreemanjobs-hash/Instruments"
}

function Invoke-GhExit {
    param([string[]]$GhArgs)
    & gh @GhArgs
    if ($LASTEXITCODE -ne 0) {
        throw "gh failed ($LASTEXITCODE): gh $($GhArgs -join ' ')"
    }
}

if (-not (Get-Command gh -ErrorAction SilentlyContinue)) {
    Write-Host "Install GitHub CLI: https://cli.github.com/ then gh auth login" -ForegroundColor Yellow
    exit 1
}

& gh auth status *> $null
if (-not $?) {
    Write-Host "Run: gh auth login" -ForegroundColor Yellow
    exit 1
}

if ([string]::IsNullOrWhiteSpace($Repo)) {
    $Repo = Get-GhRepoFromGit -Root $RepoRoot
}

Push-Location $RepoRoot
try {
    $success = $null
    if ($RunId -gt 0) {
        $success = [PSCustomObject]@{ databaseId = $RunId; headBranch = $Branch; url = "" }
    } else {
        $raw = $null
        foreach ($wf in @("nts1-mkii-build.yml", "NTS-1 mkII oscillators")) {
            try {
                $raw = gh run list -R $Repo --workflow $wf --limit 30 --json databaseId,conclusion,status,headBranch,url 2>$null
                if ($LASTEXITCODE -eq 0 -and -not [string]::IsNullOrWhiteSpace($raw) -and $raw.Trim() -ne "[]") {
                    break
                }
            } catch {}
        }
        if ([string]::IsNullOrWhiteSpace($raw) -or $raw.Trim() -eq "[]") {
            Write-Host "No workflow runs listed for $Repo (need git pull + merged workflow?)." -ForegroundColor Yellow
            Write-Host "Fix: gh workflow run nts1-mkii-build.yml -R $Repo --ref main"
            Write-Host "Or WSL: bash ./scripts/windows/build-mkii-wsl.sh"
            exit 1
        }
        $runJson = $raw | ConvertFrom-Json
        if ($runJson -isnot [array]) { $runJson = @($runJson) }
        $success = $runJson | Where-Object { $_.conclusion -eq "success" -and $_.headBranch -eq $Branch } | Select-Object -First 1
        if (-not $success) {
            $success = $runJson | Where-Object { $_.conclusion -eq "success" } | Select-Object -First 1
        }
    }

    if (-not $success) {
        Write-Host "No successful run yet. Start one:" -ForegroundColor Yellow
        Write-Host "  gh workflow run nts1-mkii-build.yml -R $Repo --ref main"
        exit 1
    }

    $runId = $success.databaseId
    Write-Host "Using run $runId (branch: $($success.headBranch))" -ForegroundColor Cyan
    if ($success.url) { Write-Host $success.url }

    $staging = Join-Path $env:TEMP "nts1-mkii-download-$runId"
    if (Test-Path $staging) { Remove-Item -Recurse -Force $staging }
    New-Item -ItemType Directory -Path $staging | Out-Null

    $downloaded = $false
    foreach ($attempt in 1..4) {
        try {
            Invoke-GhExit @("run", "download", "$runId", "-R", $Repo, "--name", $ArtifactName, "--dir", $staging)
            $downloaded = $true
            break
        } catch {
            Write-Host "Download attempt $attempt failed: $($_.Exception.Message)" -ForegroundColor Yellow
            Start-Sleep -Seconds ([int][Math]::Pow(2, $attempt))
        }
    }
    if (-not $downloaded) {
        Write-Host "gh download failed (GitHub/Azure glitch). Use the browser:" -ForegroundColor Yellow
        Write-Host "  Actions -> NTS-1 mkII oscillators -> run $runId -> Artifacts -> $ArtifactName"
        Write-Host "Unzip to: $ReleaseRoot"
        Write-Host "Or WSL: bash ./scripts/windows/build-mkii-wsl.sh"
        exit 1
    }

    New-Item -ItemType Directory -Force -Path $ReleaseRoot | Out-Null
    Copy-Item -Path (Join-Path $staging "*") -Destination $ReleaseRoot -Recurse -Force
    Write-Host "Installed under $ReleaseRoot" -ForegroundColor Green
    Write-Host "Next: .\scripts\windows\prepare-mkii-test.ps1"
} finally {
    Pop-Location
}