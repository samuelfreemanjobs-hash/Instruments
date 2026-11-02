# Pull latest handoffs from GitHub and show active Cursor → Antigravity work.
param(
  [string]$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
)

$ErrorActionPreference = "Stop"
Set-Location $RepoRoot

if (Get-Command git -ErrorAction SilentlyContinue) {
  git pull --rebase 2>$null
}

$inbox = Join-Path $RepoRoot "disklordz\antigravity\inbox"
$files = Get-ChildItem -Path $inbox -Filter "*.json" -ErrorAction SilentlyContinue | Sort-Object LastWriteTime -Descending

if (-not $files) {
  Write-Host "No inbox handoffs. Cursor will drop HO-*.json here via antigravity-bridge.sh send"
  exit 0
}

$latest = $files[0]
Write-Host "Latest handoff: $($latest.Name)"
Get-Content $latest.FullName | Write-Host

Write-Host ""
Write-Host "Open context paths from JSON in this repo, then implement on Windows."
