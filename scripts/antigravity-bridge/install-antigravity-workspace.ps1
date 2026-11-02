# Disklordz: one-time Antigravity workspace wiring (run from repo root on Windows).
param(
  [string]$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
)

$ErrorActionPreference = "Stop"
$agDir = Join-Path $RepoRoot "disklordz\antigravity"
$instructions = Join-Path $agDir "AGENT_INSTRUCTIONS.md"
$envExample = Join-Path $agDir "env.windows.example"
$envLocal = Join-Path $agDir "env.windows.local"

Write-Host "Disklordz Antigravity workspace install"
Write-Host "Repo: $RepoRoot"

if (-not (Test-Path $instructions)) {
  throw "Missing $instructions — open the Instruments repo root."
}

if (-not (Test-Path $envLocal)) {
  Copy-Item $envExample $envLocal
  Write-Host "Created $envLocal — edit HISE_EXE, HISE_SRC, PROJECT_DIR"
} else {
  Write-Host "env.windows.local already exists"
}

$cursorRules = Join-Path $RepoRoot ".cursor\rules\antigravity-hise-lane.mdc"
$rulesDir = Split-Path $cursorRules -Parent
if (-not (Test-Path $rulesDir)) { New-Item -ItemType Directory -Path $rulesDir -Force | Out-Null }

@'
---
description: Disklordz HISE lane for Antigravity — handoff inbox/outbox
globs: hise-sketch/**,disklordz/antigravity/**
---

Read disklordz/antigravity/AGENT_INSTRUCTIONS.md and docs/HISE_ANTIGRAVITY_LANE.md before HISE work.
Poll disklordz/antigravity/inbox/ after git pull.
'@ | Set-Content -Path $cursorRules -Encoding UTF8

Write-Host ""
Write-Host "Next steps:"
Write-Host "  1. Antigravity → pin AGENT_INSTRUCTIONS.md in project instructions"
Write-Host "  2. .\scripts\antigravity-bridge\sync-handoff.ps1"
Write-Host "  3. Complete WO; .\scripts\antigravity-bridge\publish-handoff.ps1"
