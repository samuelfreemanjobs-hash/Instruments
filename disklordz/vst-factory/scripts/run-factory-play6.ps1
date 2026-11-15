# PlaySynth 6-pack zero-babysitting factory (Windows)
#   pwsh -File disklordz/vst-factory/scripts/run-factory-play6.ps1
#   pwsh -File ... -Deploy:$false -SkipClaude:$true   # template-only compile + audio smoke

param(
    [string]$QueueFile = "",
    [string]$RepoRoot = "",
    [string]$TemplateDir = "",
    [int]$MaxTurns = 28,
    [switch]$Deploy = $true,
    [switch]$SkipClaude = $false
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
$UiVerify = Join-Path $PSScriptRoot "verify-play-ui-assets.ps1"
$Queue = Get-Content $QueueFile -Raw | ConvertFrom-Json
$LogsDir = Join-Path $RepoRoot "disklordz/vst-factory/logs/play6"
$OutputDir = Join-Path $RepoRoot "disklordz/vst-factory/builds_ready"
New-Item -ItemType Directory -Force -Path $LogsDir, $OutputDir | Out-Null

$Summary = @()

foreach ($Item in $Queue) {
    $Name = $Item.name
    $Code = $Item.code
    $BuildRoot = if ($Item.build_root) { $Item.build_root } else { $Name }
    $PluginDir = Join-Path $RepoRoot $BuildRoot
    $LogFile = Join-Path $LogsDir "$Name.log"
    $PreviewWav = Join-Path $LogsDir "${Name}_preview.wav"
    $Status = "FAILED_BUILD"
    $AudioStatus = "SKIPPED"
    $UiStatus = "SKIPPED"
    $Start = Get-Date

    Write-Host "`n==========================================" -ForegroundColor Cyan
    Write-Host "BUILDING: $Name ($Code)" -ForegroundColor Cyan
    Write-Host "==========================================" -ForegroundColor Cyan

    if (Test-Path $PluginDir) { Remove-Item $PluginDir -Recurse -Force }
    Copy-Item -Path $TemplateDir -Destination $PluginDir -Recurse -Force
    $cmakeTemplate = Join-Path $PluginDir "CMakeLists.template.txt"
    Copy-Item -Path $cmakeTemplate -Destination (Join-Path $PluginDir "CMakeLists.txt") -Force

    Push-Location $PluginDir
    "" | Out-File $LogFile
    & pwsh -File $UiVerify -PluginDir $PluginDir 2>&1 | Tee-Object -FilePath $LogFile -Append
    if ($LASTEXITCODE -eq 0) { $UiStatus = "UI_ASSETS_OK" }

    $cmakeArgs = @(
        "-B", "build",
        "-G", "Visual Studio 17 2022",
        "-A", "x64",
        "-DPLUGIN_NAME=$Name",
        "-DPLUGIN_CODE=$Code"
    )
    Write-Host "CMake configure..." -ForegroundColor Cyan
    cmake @cmakeArgs 2>&1 | Tee-Object -FilePath $LogFile -Append
    cmake --build build --config Release --parallel 2>&1 | Tee-Object -FilePath $LogFile -Append

    $Vst3Path = Get-ChildItem -Path "build" -Recurse -Filter "*.vst3" -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty FullName
    $Standalone = Get-ChildItem -Path "build" -Recurse -Filter "*.exe" -ErrorAction SilentlyContinue |
        Where-Object { $_.Name -match [regex]::Escape($Name) } | Select-Object -First 1

    if ($Vst3Path) { $Status = "COMPILED" }

    $PluginVal = Join-Path $RepoRoot "tools/pluginval.exe"
    if ($Vst3Path -and (Test-Path $PluginVal)) {
        & $PluginVal --strictness-level 5 --validate-in-process --validate "$Vst3Path" 2>&1 |
            Tee-Object -FilePath $LogFile -Append
        if ($LASTEXITCODE -eq 0) {
            $Status = "PASSED_PLUGINVAL_5"
        } else {
            $Status = "FAILED_PLUGINVAL"
        }
    }

    if ($Standalone -and (Test-Path $Standalone.FullName)) {
        if (Test-Path $PreviewWav) { Remove-Item $PreviewWav -Force }
        & $Standalone.FullName --render-test 2>&1 | Tee-Object -FilePath $LogFile -Append
        $tempWav = Join-Path $env:TEMP "play_render_preview.wav"
        if (Test-Path $tempWav) {
            Copy-Item $tempWav $PreviewWav -Force
            $AudioStatus = "RENDER_OK"
        } elseif ($LASTEXITCODE -eq 0) {
            $AudioStatus = "RENDER_OK"
        } else {
            $AudioStatus = "RENDER_FAIL"
            if ($Status -eq "PASSED_PLUGINVAL_5") { $Status = "AUDIO_SMOKE_FAIL" }
        }
    }

    $claudeCmd = Get-Command claude -ErrorAction SilentlyContinue
    if (-not $SkipClaude -and $claudeCmd -and $Status -match "COMPILED|PASSED") {
        $CustomizePrompt = @"
The PlaySynth **core** already compiles (APVTS, WebView Assets, voices, arp/scale, render smoke).
Only customize these files for '$Name' (PLUGIN_CODE $Code):
- Source/LayerAEngine.h (Layer A DSP)
- Source/LayerBEngine.h (Layer B DSP)
- Source/MacroRouter.h (macro curves if needed)
- Assets/index.html, Assets/style.css, Assets/app.js (theme: $($Item.theme))
- Source/FactoryPresets.h names + 12 blend/macro rows matching: $($Item.presets)

Do NOT rewrite PluginProcessor editor/CMake unless build breaks.
THEME: $($Item.theme)
LAYER A: $($Item.layerA)
LAYER B: $($Item.layerB)
MACROS: $($Item.macros)

$Contract

Rebuild Release and ensure pluginval + --render-test still pass.
"@
        claude -p $CustomizePrompt `
            --dangerously-skip-permissions `
            --max-turns $MaxTurns `
            --allowedTools "Bash,Read,Write,Edit,Glob,Grep" `
            2>&1 | Tee-Object -FilePath $LogFile -Append

        cmake --build build --config Release --parallel 2>&1 | Tee-Object -FilePath $LogFile -Append
        $Vst3Path = Get-ChildItem -Path "build" -Recurse -Filter "*.vst3" -ErrorAction SilentlyContinue |
            Select-Object -First 1 -ExpandProperty FullName

        if ($Vst3Path -and (Test-Path $PluginVal)) {
            & $PluginVal --strictness-level 5 --validate-in-process --validate "$Vst3Path" 2>&1 |
                Tee-Object -FilePath $LogFile -Append
            if ($LASTEXITCODE -eq 0) { $Status = "PASSED_LEVEL_5_CUSTOMIZED" }
        }
    } elseif (-not $SkipClaude -and -not $claudeCmd) {
        $CustomizePrompt = "See template-only build. Customize LayerA/B and Assets manually."
        $CustomizePrompt | Out-File (Join-Path $LogsDir "$Name.customize.prompt.txt")
    }

    if ($Vst3Path -and $Status -match "PASSED|COMPILED") {
        Copy-Item -Path $Vst3Path -Destination $OutputDir -Recurse -Force
        if ($Deploy) {
            $dest = "C:\Program Files\Common Files\VST3"
            if (Test-Path $dest) {
                Copy-Item -Path $Vst3Path -Destination $dest -Recurse -Force -ErrorAction SilentlyContinue
            }
        }
    }

    $Summary += [PSCustomObject]@{
        Plugin      = $Name
        Code        = $Code
        Status      = $Status
        AudioSmoke  = $AudioStatus
        UiContract  = $UiStatus
        PreviewWav  = if (Test-Path $PreviewWav) { $PreviewWav } else { "" }
        Minutes     = [math]::Round(((Get-Date) - $Start).TotalMinutes, 1)
    }

    Pop-Location
}

Write-Host "`n================ MORNING FACTORY REPORT ================" -ForegroundColor Green
$Summary | Format-Table -AutoSize
$reportCsv = Join-Path $LogsDir "morning_report.csv"
$Summary | Export-Csv -Path $reportCsv -NoTypeInformation
Write-Host "CSV: $reportCsv" -ForegroundColor Cyan