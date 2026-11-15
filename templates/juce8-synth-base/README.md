# JUCE 8 desktop synth factory template

First-pass **PlaySynth Trilogy** scaffold: dual-layer shell, XY + Dice, 8 macros, BinaryData-ready CMake.

## Quick start (per plugin)

1. Copy entire `templates/juce8-synth-base/` to repo root as `AnalogDreams84/` (or `CloudSupplyTrap` / `LoFiGlowTape`).
2. Rename `CMakeLists.template.txt` → `CMakeLists.txt`; match `PLUGIN_CODE` (`Ad84`, `CsTr`, `LgTp`).
3. Search-replace `PlaySeriesTemplate` → `AnalogDreams84` in `Source/*.h` / `*.cpp`.
4. Add `add_subdirectory(AnalogDreams84)` to root `CMakeLists.txt` (see `ProphetRev2Trap`).
5. Drop 12 preset JSON under `Resources/presets/`; extend `juce_add_binary_data` in CMake.
6. Replace `processBlock` silence with `Source/AnalogDreams84Engine.h` (plugin-specific DSP).

```powershell
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target AnalogDreams84_VST3
```

## Files

| Path | Purpose |
|------|---------|
| `Source/PlaySeriesCommon.h` | Equal-power blend, shared APVTS IDs |
| `Source/PluginProcessor.*` | APVTS, presets, bounded Dice |
| `Source/PluginEditor.*` | WebView host + Dice button |
| `Source/FactoryPresets.h` | 12-slot placeholder table (replace) |
| `CMakeLists.template.txt` | WebView + BinaryData example |
| `BinaryData.cmake.snippet` | Minimal preset embedding |

## Overnight queue (trilogy)

See `plugins/PLAY_SYNTH_TRILOGY.md` and `disklordz/vst-factory/data/overnight-queue.json` (`suite: play_synth_trilogy`, priority 10).

## Other factory SKUs

| Plugin | CODE | Voices |
|--------|------|--------|
| ProphetMod16 | Pm16 | 16 |
| OxfordPeak8 | Opk8 | 8 |

There is no in-repo `run-factory.ps1`; point your runner at `overnight-queue.json` or paste prompts manually on Windows.