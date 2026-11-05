# Style -> 100 VST3 one-shots for MPC (Windows).
# Usage:
#   .\scripts\boom.ps1 -Style "dark melodic stabs" -Vst3 "C:\Program Files\Common Files\VST3\Serum.vst3" -Out "$env:USERPROFILE\Exports\mpc_pack"
param(
    [Parameter(Mandatory = $true)]
    [string]$Style,
    [Parameter(Mandatory = $true)]
    [string]$Vst3,
    [Parameter(Mandatory = $true)]
    [string]$Out,
    [int]$Count = 100,
    [ValidateSet("names", "preview", "hybrid")]
    [string]$MatchMode = "hybrid",
    [switch]$DryRun
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$BuildDir = Join-Path $Root "build"

function Find-RenderBin {
    $candidates = @(
        (Join-Path $BuildDir "Release\VstiOneShotRender.exe"),
        (Join-Path $BuildDir "VstiOneShotRender.exe"),
        (Join-Path $BuildDir "Debug\VstiOneShotRender.exe")
    )
    foreach ($c in $candidates) {
        if (Test-Path -LiteralPath $c) { return $c }
    }
    return $null
}

$RenderBin = $env:VSTI_ONE_SHOT_RENDER_BIN
if (-not $RenderBin) {
    $RenderBin = Find-RenderBin
}

if (-not $RenderBin) {
    Write-Host "Building VstiOneShotRender (Release)..."
    Push-Location $Root
    try {
        cmake -B build -DCMAKE_BUILD_TYPE=Release
        cmake --build build --config Release --target VstiOneShotRender -j
    } finally {
        Pop-Location
    }
    $RenderBin = Find-RenderBin
    if (-not $RenderBin) {
        throw "VstiOneShotRender.exe not found under $BuildDir after build."
    }
}

$Python = $null
foreach ($cmd in @("py -3", "python", "python3")) {
    $exe = $cmd.Split(" ")[0]
    if (Get-Command $exe -ErrorAction SilentlyContinue) {
        $Python = $cmd
        break
    }
}
if (-not $Python) {
    throw "Python 3 not found. Install from https://www.python.org/downloads/ (check 'Add to PATH')."
}

$Boom = Join-Path $Root "tools\vsti-one-shot\boom.py"
$ArgsList = @(
    $Boom,
    "--style", $Style,
    "--vst3", $Vst3,
    "--out", $Out,
    "--count", "$Count",
    "--match-mode", $MatchMode,
    "--render-bin", $RenderBin
)
if ($DryRun) { $ArgsList += "--dry-run" }

if ($Python -eq "py -3") {
    & py -3 @ArgsList
} else {
    & $Python @ArgsList
}
exit $LASTEXITCODE
