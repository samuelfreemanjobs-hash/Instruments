# Antigravity → Cursor: write outbox handoff (commit/push separately).
param(
  [Parameter(Mandatory)][string]$WorkOrderId,
  [Parameter(Mandatory)][ValidateSet("open","in_progress","done","blocked")][string]$Status,
  [string]$Direction = "antigravity_to_cursor",
  [string]$Notes = "",
  [string]$ArtifactPath = "",
  [string]$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
)

$ErrorActionPreference = "Stop"
$outbox = Join-Path $RepoRoot "disklordz\antigravity\outbox"
New-Item -ItemType Directory -Force -Path $outbox | Out-Null

$day = Get-Date -Format "yyyyMMdd"
$rand = -join ((48..57 + 65..90) | Get-Random -Count 4 | ForEach-Object { [char]$_ })
$id = "HO-$day-$rand"
$created = (Get-Date).ToUniversalTime().ToString("yyyy-MM-ddTHH:mm:ssZ")

$artifacts = @()
if ($ArtifactPath) {
  $artifacts += @{ kind = "vst3_or_log"; path = $ArtifactPath; notes = $Notes }
}

$doc = @{
  handoff_id = $id
  direction = $Direction
  from_agent = "antigravity-hise"
  to_agent = "cursor-cloud"
  created_at = $created
  work_order = @{
    id = $WorkOrderId
    title = $WorkOrderId
  }
  context_paths = @("docs/HISE_ANTIGRAVITY_LANE.md")
  artifacts = $artifacts
  status = $Status
  notes = $Notes
}

$file = Join-Path $outbox "$id.json"
$doc | ConvertTo-Json -Depth 6 | Set-Content -Path $file -Encoding UTF8
Write-Host "Wrote $file"
Write-Host "Commit and push, or comment on the GitHub issue with this path."
