# Auto Sampler — architecture

JUCE **VST3 + Standalone** instrument modeled after **MPC Software Auto Sampler**: capture multisamples from **stereo audio input** while sequencing MIDI notes (optional **MIDI output** to drive a source instrument), play back mapped zones, and **export** preset folders or whole **preset banks** for batch workflows.

## Purpose

Musicians and agents who need **repeatable multisample capture** without manual slice-by-slice recording: route a source synth into the plugin input, arm auto-sample over a note range, then export portable preset packs or automate batch export via CLI.

## Build & run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target AutoSampler_VST3 AutoSampler_Standalone AutoSamplerBatchExport AutoSamplerTests
ctest --test-dir build -R AutoSampler --output-on-failure
```

Artifacts:

- VST3: `build/AutoSampler/AutoSampler_artefacts/Release/VST3/Auto Sampler.vst3`
- Standalone: `build/AutoSampler/AutoSampler_artefacts/Release/Standalone/Auto Sampler`
- Batch CLI: `build/AutoSampler/AutoSamplerBatchExport`

### Windows prebuild (no local compile)

GitHub Actions workflow **Auto Sampler Windows prebuild** (`.github/workflows/autosampler-windows.yml`) produces **`AutoSampler-Windows-x64.zip`**:

1. Open the repo on GitHub → **Actions** → **Auto Sampler Windows prebuild**.
2. Select the latest run for your branch (e.g. `cursor/autosampler-vst3-a429`) or use **Run workflow**.
3. Download **AutoSampler-Windows-x64** from **Artifacts** at the bottom of a green run.

Zip layout: `VST3/`, `Standalone/`, `Tools/AutoSamplerBatchExport.exe`, `examples/`, `README.txt`.

## Data flow

```text
Source synth (external) ──audio──► Auto Sampler input bus
Auto Sampler auto-record ──MIDI──► Source synth (when MIDI out enabled)
Captured buffers ──► MultisampleProgram (zones per root note)
MIDI in ──► SamplerEngine (zone map + pitch playback) ──► stereo out
Export ──► folder: preset.json + `<name>.sfz` + samples/*.wav  (or bank manifest)
Capture finalize ──► trim silence, peak normalize, zero-cross sustain loop points
```

## Threading / realtime

| Thread | Work |
|--------|------|
| Audio | `processBlock`: append input while recording; voice mix; **no heap alloc** on steady-state playback |
| Message | Editor, file chooser, export jobs kicked from UI |
| Background | Optional future: async WAV encode (currently export on message thread from UI/CLI) |

## Key modules

| Path | Responsibility |
|------|----------------|
| `Source/Engine/MultisampleProgram.h` | Zones, root notes, key ranges |
| `Source/Engine/AutoRecordEngine.*` | Note schedule, input capture, MIDI thru |
| `Source/Engine/SamplerEngine.*` | Voice playback with pitch ratio |
| `Source/Engine/SamplePostProcessor.*` | Trim, normalize, sustain loop detection |
| `Source/Export/MultisampleExporter.*` | JSON + SFZ + WAV; bank export |
| `Source/PluginProcessor.*` | APVTS, programs (bank slots), export API |
| `Source/PluginEditor.*` | Range controls, arm/export buttons |
| `tools/AutoSamplerBatchExport.cpp` | Headless batch bank export |

## Extension points

- Add velocity layers in `MultisampleProgram::Zone`
- EXS24 export in `MultisampleExporter`
- Smarter loop scoring (cross-correlation) in `SamplePostProcessor`
- Host automation: `startAutoSample` and `exportPreset` bool parameters (momentary)

## Related docs

- [../ARCHITECTURE.md](../ARCHITECTURE.md) — monorepo index
- [../SP1200/ARCHITECTURE.md](../SP1200/ARCHITECTURE.md) — input recording reference
