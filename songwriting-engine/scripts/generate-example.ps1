$ErrorActionPreference = "Stop"
$EngineRoot = Split-Path -Parent $PSScriptRoot
$PythonCommand = Get-Command python3.12, python -ErrorAction SilentlyContinue | Select-Object -First 1
if (-not $PythonCommand) { throw "Python 3.12+ is required" }
Push-Location $EngineRoot
try {
    & $PythonCommand.Source -m songwriting.cli generate --config .\configs\smoker_sketch.json --output-dir .\artifacts
    if ($LASTEXITCODE -ne 0) { throw "Song generation or MIDI validation failed" }
} finally {
    Pop-Location
}