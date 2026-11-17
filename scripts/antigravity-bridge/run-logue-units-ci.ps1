# Windows wrapper — calls scripts/trigger-logue-units-build.sh (Git Bash / WSL).
param(
    [string]$Ref = "",
    [switch]$Watch,
    [switch]$Download,
    [switch]$Local
)

$ErrorActionPreference = "Stop"
$Root = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$bash = Get-Command bash -ErrorAction SilentlyContinue
if (-not $bash) {
    throw "bash not found (install Git for Windows)."
}

$argsList = @()
if ($Ref) { $argsList += "--ref"; $argsList += $Ref }
if ($Watch) { $argsList += "--watch" }
if ($Download) { $argsList += "--download" }
if ($Local) { $argsList += "--local" }

$argStr = ($argsList -join ' ')
$rootUnix = $Root.Path -replace '\\', '/'
& $bash.Source -lc "cd '$rootUnix' && ./scripts/trigger-logue-units-build.sh $argStr"
if ($LASTEXITCODE -ne 0) { throw "trigger-logue-units-build.sh failed" }
