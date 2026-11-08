# Active mode: pull handoffs and optionally loop until idle (Windows / Antigravity).
param(
  [string]$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path,
  [int]$IntervalSeconds = 900,
  [switch]$Once
)

$ErrorActionPreference = "Stop"
$inbox = Join-Path $RepoRoot "disklordz\antigravity\inbox"
$queue = Join-Path $RepoRoot "disklordz\antigravity\active-queue.json"
$instructions = Join-Path $RepoRoot "disklordz\antigravity\AGENT_INSTRUCTIONS.md"

function Show-Handoffs {
  Set-Location $RepoRoot
  if (Get-Command git -ErrorAction SilentlyContinue) {
    git pull --rebase 2>$null
  }
  $files = Get-ChildItem -Path $inbox -Filter "HO-*.json" -ErrorAction SilentlyContinue |
    Sort-Object LastWriteTime -Descending
  if (-not $files) {
    Write-Host "[Antigravity] No inbox handoffs — waiting for Cursor dispatch-active or bridge send."
    return
  }
  foreach ($f in $files) {
    $json = Get-Content $f.FullName | ConvertFrom-Json
  $status = $json.status
    if ($status -eq "done") { continue }
    Write-Host "=== ACTIVE: $($f.Name) — $($json.work_order.title) ===" -ForegroundColor Cyan
    Write-Host "WO: $($json.work_order.id)  Branch: $($json.work_order.branch)"
    Write-Host ($json.work_order.acceptance_criteria)
    Write-Host "Context:"
    foreach ($p in $json.context_paths) { Write-Host "  - $p" }
    Write-Host ""
  }
  Write-Host "Pinned instructions: $instructions"
  Write-Host "When finished: .\scripts\antigravity-bridge\publish-handoff.ps1 ..."
}

do {
  Show-Handoffs
  if ($Once) { break }
  Write-Host "Next pull in $IntervalSeconds s (Ctrl+C to stop)"
  Start-Sleep -Seconds $IntervalSeconds
} while ($true)
