$ErrorActionPreference = "Stop"
$DesktopRoot = Join-Path (Split-Path -Parent $PSScriptRoot) "apps\desktop"
Push-Location $DesktopRoot
try {
    npm.cmd run desktop:dev
    if ($LASTEXITCODE -ne 0) { throw "Desktop development app exited with an error" }
} finally {
    Pop-Location
}