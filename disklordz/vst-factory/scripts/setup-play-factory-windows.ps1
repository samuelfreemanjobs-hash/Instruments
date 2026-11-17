# One-time Windows prep for PlaySynth 6-pack factory
param(
  [string]$RepoRoot = ""
)

if (-not $RepoRoot) {
  $RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "../../..")).Path
}

Write-Host "Repo: $RepoRoot" -ForegroundColor Cyan

Push-Location $RepoRoot
try {
  Write-Host "Updating git submodules (vst/JUCE)..." -ForegroundColor Cyan
  git submodule update --init --depth 1 vst/JUCE 2>$null
  if (-not (Test-Path "vst/JUCE/CMakeLists.txt")) {
    Write-Warning "vst/JUCE missing. Run: git submodule update --init vst/JUCE"
  }

  $tools = Join-Path $RepoRoot "tools"
  New-Item -ItemType Directory -Force -Path $tools | Out-Null
  $pluginVal = Join-Path $tools "pluginval.exe"
  if (-not (Test-Path $pluginVal)) {
    Write-Host "Downloading pluginval (Tracktion)..." -ForegroundColor Cyan
    $zip = Join-Path $env:TEMP "pluginval_win.zip"
    Invoke-WebRequest -Uri "https://github.com/Tracktion/pluginval/releases/latest/download/pluginval_windows.zip" -OutFile $zip -UseBasicParsing
    Expand-Archive -Path $zip -DestinationPath $tools -Force
    Get-ChildItem -Path $tools -Recurse -Filter "pluginval.exe" | Select-Object -First 1 | ForEach-Object {
      Copy-Item $_.FullName $pluginVal -Force
    }
  } else {
    Write-Host "pluginval already present at tools/pluginval.exe" -ForegroundColor Green
  }

  Write-Host "Pragma validate master queue JSON..." -ForegroundColor Cyan
  $master = Join-Path $RepoRoot "disklordz/vst-factory/data/play_synth_6pack_master.json"
  if (Test-Path $master) {
    Get-Content $master -Raw | ConvertFrom-Json | Out-Null
    Write-Host "play_synth_6pack_master.json OK" -ForegroundColor Green
  }
} finally {
  Pop-Location
}

Write-Host "Done. Next: pwsh disklordz/vst-factory/scripts/run-factory-play6.ps1" -ForegroundColor Green