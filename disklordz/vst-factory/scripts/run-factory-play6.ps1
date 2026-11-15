# PlaySynth 6-pack overnight builder (Windows + Claude Code)
# Usage (from repo root):
#   pwsh -File disklordz/vst-factory/scripts/run-factory-play6.ps1
#   pwsh -File disklordz/vst-factory/scripts/run-factory-play6.ps1 -Deploy:$false

param(
    [string]$QueueFile = "",
    [string]$RepoRoot = "",
    [string]$TemplateDir = "",
    [int]$MaxTurns = 40,
    [switch]$Deploy = $true
)

$ErrorActionPreference = "Continue"

if (-not $RepoRoot) {
    $RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "../../..")).Path
}
if (-not $QueueFile) {
    $QueueFile = Join-Path $RepoRoot "disklordz/vst-factory/data/play_synth_6pack_master.json"
}
if (-not $TemplateDir) {
    $TemplateDir = Join-Path $RepoRoot "templates/juce8-synth-base"
}

$Contract = Get-Content (Join-Path $RepoRoot "disklordz/vst-factory/data/play_synth_factory_contract.md") -Raw
$Queue = Get-Content $QueueFile -Raw | ConvertFrom-Json
$LogsDir = Join-Path $RepoRoot "disklordz/vst-factory/logs/play6"
New-Item -ItemType Directory -Force -Path $LogsDir | Out-Null

foreach ($Item in $Queue) {
    $Name = $Item.name
    $BuildRoot = if ($Item.build_root) { $Item.build_root } else { $Name }
    $PluginDir = Join-Path $RepoRoot $BuildRoot
    $LogFile = Join-Path $LogsDir "$Name.log"

    Write-Host "`n==========================================" -ForegroundColor Cyan
    Write-Host "BUILDING: $Name ($($Item.code)) -> $BuildRoot" -ForegroundColor Cyan
    Write-Host "==========================================" -ForegroundColor Cyan

    if (Test-Path $PluginDir) { Remove-Item $PluginDir -Recurse -Force }
    Copy-Item -Path $TemplateDir -Destination $PluginDir -Recurse -Force
    Push-Location $PluginDir

    $FullPrompt = @"
Build a 12-voice dual-layer VST3 '$Name' (PLUGIN_CODE $($Item.code)) at repo root folder '$BuildRoot'.
Read APC: $($Item.apc_path) and plugins/PLAY_SYNTH_6PACK.md. Follow AGENTS.md and docs/JUCE8_VST3_PROJECT_RULES.md.

LAYER A: $($Item.layerA)
LAYER B: $($Item.layerB)
THEME: $($Item.theme)
MACROS: $($Item.macros)
FACTORY PRESETS (BinaryData x12): $($Item.presets)

$Contract

CMake: copy CMakeLists.template.txt to CMakeLists.txt, wire juce submodule at vst/JUCE or fetch JUCE 8 per parent repo.
Build: cmake -B build -G "Visual Studio 17 2022" -A x64; cmake --build build --config Release --parallel
Output 'BUILD_COMPLETE' when .vst3 exists under build/
"@

    $claudeCmd = Get-Command claude -ErrorAction SilentlyContinue
    if ($claudeCmd) {
        claude -p $FullPrompt `
            --dangerously-skip-permissions `
            --max-turns $MaxTurns `
            --allowedTools "Bash,Read,Write,Edit,Glob,Grep" `
            *> $LogFile
    } else {
        "Claude CLI not found; prompt written only." | Out-File $LogFile
        $FullPrompt | Out-File (Join-Path $LogsDir "$Name.prompt.txt")
        Write-Warning "Install claude CLI or paste $($LogsDir)/$Name.prompt.txt into Cursor/Claude Code."
    }

    $Vst3Path = Get-ChildItem -Path "build" -Recurse -Filter "*.vst3" -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty FullName

    $PluginVal = Join-Path $RepoRoot "tools/pluginval.exe"
    if ($Vst3Path -and (Test-Path $PluginVal)) {
        & $PluginVal --strictness-level 5 --validate-in-process --validate "$Vst3Path" *>> $LogFile
        if ($LASTEXITCODE -ne 0 -and $claudeCmd) {
            Write-Warning "[RETRY] $Name failed pluginval; feeding log to Claude..."
            $Fix = "pluginval strictness 5 failed for $Name. Read tail of $LogFile, fix RT/state bugs, rebuild Release."
            claude -p $Fix --dangerously-skip-permissions --max-turns 15 --allowedTools "Bash,Read,Write,Edit,Glob,Grep" *>> $LogFile
            & $PluginVal --strictness-level 5 --validate-in-process --validate "$Vst3Path" *>> $LogFile
        }
    }

    if ($Vst3Path) {
        Write-Host "[OK] $Vst3Path" -ForegroundColor Green
        if ($Deploy) {
            $dest = "C:\Program Files\Common Files\VST3"
            if (Test-Path $dest) {
                Copy-Item -Path $Vst3Path -Destination $dest -Recurse -Force
            } else {
                $out = Join-Path $RepoRoot "disklordz/vst-factory/output"
                New-Item -ItemType Directory -Force -Path $out | Out-Null
                Copy-Item -Path $Vst3Path -Destination $out -Recurse -Force
            }
        }
    } else {
        Write-Host "[FAIL] No .vst3 — see $LogFile" -ForegroundColor Red
    }

    Pop-Location
}

Write-Host "`nPlaySynth 6-pack batch done. Logs: $LogsDir" -ForegroundColor Cyan