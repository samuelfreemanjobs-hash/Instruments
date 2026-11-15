$ErrorActionPreference = "Stop"
$EngineRoot = Split-Path -Parent $PSScriptRoot
$PythonCommand = Get-Command python3.12, python -ErrorAction SilentlyContinue | Select-Object -First 1
if (-not $PythonCommand) {
    throw "Python 3.12+ is required. Install it from https://www.python.org/downloads/windows/"
}
$Cargo = Join-Path $env:USERPROFILE ".cargo\bin\cargo.exe"
if (-not (Test-Path $Cargo)) {
    $Installer = Join-Path $env:TEMP "rustup-init.exe"
    Invoke-WebRequest -Uri "https://static.rust-lang.org/rustup/archive/1.29.1/x86_64-pc-windows-msvc/rustup-init.exe" -OutFile $Installer
    $Hash = (Get-FileHash $Installer -Algorithm SHA256).Hash.ToLower()
    if ($Hash -ne "6f4bef66261261fcb43131be8720bab817d403a09edec7455c371974b90bdb7e") {
        throw "Rustup installer SHA-256 verification failed"
    }
    & $Installer -y --profile minimal --default-host x86_64-pc-windows-msvc --default-toolchain stable
    if ($LASTEXITCODE -ne 0) { throw "Rust toolchain installation failed" }
}
$env:PATH = "$(Join-Path $env:USERPROFILE '.cargo\bin');$env:PATH"
$DesktopRoot = Join-Path $EngineRoot "apps\desktop"
Push-Location $EngineRoot
try {
    & $PythonCommand.Source -m pip install -r .\requirements.lock
    if ($LASTEXITCODE -ne 0) { throw "Pinned Python dependency installation failed" }
    & $PythonCommand.Source -m pip install --no-deps -e .
    if ($LASTEXITCODE -ne 0) { throw "Python dependency installation failed" }
} finally {
    Pop-Location
}
Push-Location $DesktopRoot
try {
    npm.cmd ci
    if ($LASTEXITCODE -ne 0) { throw "Desktop dependency installation failed" }
} finally {
    Pop-Location
}
& (Join-Path $PSScriptRoot "build-engine.ps1")
if ($LASTEXITCODE -ne 0) { throw "Engine sidecar build failed" }
Write-Host "Songwriting engine setup complete. Run scripts/test.ps1, then scripts/dev.ps1."