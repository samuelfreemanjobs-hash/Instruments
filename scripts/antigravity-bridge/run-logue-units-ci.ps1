# Trigger GitHub Actions workflow "logue NTS-1 units" (Antigravity / Windows).
# Usage: .\scripts\antigravity-bridge\run-logue-units-ci.ps1 [-Ref branch-or-sha] [-Watch]
param(
    [string]$Ref = "",
    [switch]$Watch
)

$ErrorActionPreference = "Stop"
$Repo = "samuelfreemanjobs-hash/Instruments"
$Workflow = "logue NTS-1 units"

if (-not (Get-Command gh -ErrorAction SilentlyContinue)) {
    throw "GitHub CLI (gh) not found. Install from https://cli.github.com/ then gh auth login"
}

if (-not $Ref) {
    $Ref = "main"
    $branch = git -C (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path branch --show-current 2>$null
    if ($branch -match "cursor/logue-osc-build") {
        $Ref = $branch.Trim()
    }
    $merged = gh pr view 113 --repo $Repo --json state -q .state 2>$null
    if ($merged -ne "MERGED") {
        $defaultFeature = "cursor/logue-osc-build-windows-13db"
        Write-Host "PR #113 not merged; defaulting ref to $defaultFeature"
        $Ref = $defaultFeature
    }
}

Write-Host "Dispatching '$Workflow' on $Repo @ ref=$Ref"
gh workflow run $Workflow --repo $Repo --ref $Ref

if ($Watch) {
    Start-Sleep -Seconds 5
    gh run watch --repo $Repo --exit-status
} else {
    Write-Host "Run: gh run list --repo $Repo --workflow=`"$Workflow`" --limit 3"
}
