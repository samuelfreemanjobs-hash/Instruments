# Auto-start Renaissance box sidecar (edit REPO_ROOT)
$ErrorActionPreference = "Stop"
$REPO_ROOT = "C:\instruments"
$HostDir = Join-Path $REPO_ROOT "renaissance-box\host"
$Config = Join-Path $REPO_ROOT "renaissance-box\config\box.local.json"
$Python = Join-Path $HostDir ".venv\Scripts\python.exe"
$Sidecar = Join-Path $HostDir "sidecar.py"

if (-not (Test-Path $Python)) {
    Write-Error "Missing venv at $HostDir\.venv — run pip install per host/README.md"
}
if (-not (Test-Path $Config)) {
    Write-Error "Missing $Config — copy from box.windows.json"
}

Set-Location $HostDir
& $Python $Sidecar --config $Config
