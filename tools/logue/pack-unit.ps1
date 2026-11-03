param(
  [Parameter(Mandatory = $true)][string]$Project,
  [Parameter(Mandatory = $true)][string]$BuildDir,
  [Parameter(Mandatory = $true)][string]$Manifest,
  [Parameter(Mandatory = $true)][string]$OutDir
)

$ErrorActionPreference = "Stop"

function Escape-Json([string]$s) {
  return ($s -replace '\\', '\\' -replace '"', '\"')
}

$raw = Get-Content -Raw -Path $Manifest | ConvertFrom-Json
$header = $raw.header
if (-not $header) {
  throw "manifest is missing header: $Manifest"
}

$paramList = @($header.params)
if ($paramList.Count -gt 6) {
  $paramList = @($paramList | Select-Object -First 6)
}

$lines = @()
$lines += "{"
$lines += "  `"header`": {"
$lines += "    `"platform`": `"$($header.platform)`","
$lines += "    `"module`": `"$($header.module)`","
$lines += "    `"api`": `"$($header.api)`","
$lines += "    `"dev_id`": $([int]$header.dev_id),"
$lines += "    `"prg_id`": $([int]$header.prg_id),"
$lines += "    `"version`": `"$($header.version)`","
$lines += "    `"name`": `"$(Escape-Json ([string]$header.name))`","
$lines += "    `"num_param`": $($paramList.Count),"
$lines += "    `"params`": ["
for ($i = 0; $i -lt $paramList.Count; $i++) {
  $p = @($paramList[$i])
  $comma = $(if ($i -lt ($paramList.Count - 1)) { "," } else { "" })
  $lines += "      [`"$(Escape-Json ([string]$p[0]))`", $([int]$p[1]), $([int]$p[2]), `"$(Escape-Json ([string]$p[3]))`"]$comma"
}
$lines += "    ]"
$lines += "  }"
$lines += "}"

$stage = Join-Path $BuildDir "pkg"
$unitDir = Join-Path $stage $Project
if (Test-Path $stage) {
  Remove-Item -Recurse -Force $stage
}
New-Item -ItemType Directory -Path $unitDir | Out-Null

$utf8 = New-Object System.Text.UTF8Encoding $false
[System.IO.File]::WriteAllText((Join-Path $unitDir "manifest.json"), ($lines -join "`n") + "`n", $utf8)

$payloadSrc = Join-Path $BuildDir "$Project.bin"
if (-not (Test-Path $payloadSrc)) {
  throw "missing payload: $payloadSrc"
}
Copy-Item $payloadSrc (Join-Path $unitDir "payload.bin")

$archive = Join-Path $BuildDir "$Project.ntkdigunit"
$zip = Join-Path $BuildDir "$Project.zip"
if (Test-Path $archive) { Remove-Item -Force $archive }
if (Test-Path $zip) { Remove-Item -Force $zip }
# .ntkdigunit must be a zip. tar -a only writes zip when the name ends in .zip;
# writing straight to .ntkdigunit produced a tar archive logue-cli will not open.
$tar = Join-Path $env:SystemRoot "System32\tar.exe"
& $tar -a -cf $zip -C $stage $Project
if ($LASTEXITCODE -ne 0) {
  throw "tar failed while packaging $Project"
}
Move-Item -Force $zip $archive

Copy-Item $archive (Join-Path $OutDir "$Project.ntkdigunit") -Force
Write-Host "Packaged $(Join-Path $OutDir "$Project.ntkdigunit")"
