# JUCE 8 PlaySynth Core (pre-wired factory template)

Compiles as a **12-voice dual-layer** instrument with working audio, WebView chrome, arp/chord/scale lock, macro lock, 2× master saturation, pluginval-friendly APVTS, and **`--render-test`** on the Standalone target.

## Quick start (Windows)

```powershell
cd templates/juce8-synth-base
copy CMakeLists.template.txt CMakeLists.txt
cmake -B build -G "Visual Studio 17 2022" -A x64 -DPLUGIN_NAME=PlaySynthBase -DPLUGIN_CODE=PsBs
cmake --build build --config Release --parallel
```

Smoke (no GUI):

```powershell
.\build\PlaySynthBase_artefacts\Release\Standalone\PlaySynthBase.exe --render-test
```

Batch all six SKUs:

```powershell
pwsh -File disklordz/vst-factory/scripts/setup-play-factory-windows.ps1
pwsh -File disklordz/vst-factory/scripts/run-factory-play6.ps1
```

## What Claude should edit per SKU

Only **five surfaces** (see `play_synth_factory_contract.md`):

1. `Source/LayerAEngine.h`
2. `Source/LayerBEngine.h`
3. `Source/MacroRouter.h` (optional curves)
4. `Assets/index.html`, `style.css`, `app.js`
5. `Source/FactoryPresets.h` + `Assets/Presets/*.xml`

## Core modules (leave alone unless fixing bugs)

| Path | Role |
|------|------|
| `Source/PluginProcessor.*` | Synthesiser, performance engine, oversampling |
| `Source/PlaySynthVoice.*` | Dual-layer voice + ladder filter |
| `Source/PerformanceEngine.*` | Arp, chord, scale lock, MIDI export |
| `Source/RenderSmokeTest.*` | C minor 9 · 2 s offline render + WAV |
| `Source/StandaloneCustomApp.cpp` | `--render-test` entry |
| `Source/ScopeRingBuffer.h` | Lock-free peaks for UI |
| `Assets/` | WebView + preset XML (BinaryData) |

## Factory data

- Compact queue: `disklordz/vst-factory/data/play_synth_6pack_master.json`
- Docs index: `docs/VST_AI_FACTORY_INDEX.md`