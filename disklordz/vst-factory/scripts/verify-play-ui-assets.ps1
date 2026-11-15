# Lightweight UI asset verification (no GUI screenshot — checks SVG/chrome contract)
param(
  [string]$PluginDir
)

$assets = Join-Path $PluginDir "Assets"
$index = Join-Path $assets "index.html"
$css = Join-Path $assets "style.css"
$fail = $false

foreach ($path in @($index, $css)) {
  if (-not (Test-Path $path)) {
    Write-Warning "Missing $path"
    $fail = $true
    continue
  }
}

if (Test-Path $index) {
  $html = Get-Content $index -Raw
  if ($html -notmatch "svg" -or $html -notmatch "knob") {
    Write-Warning "index.html should include SVG knob markup"
    $fail = $true
  }
}

if ($fail) { exit 2 }
Write-Host "UI asset contract OK for $PluginDir" -ForegroundColor Green
exit 0