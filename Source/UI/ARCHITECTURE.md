# JD Upgraded — UI layer

Host-facing editor code lives in `PluginEditor.*` with reusable components under `Source/UI/`.

## Components

| File | Role |
|------|------|
| `WavePreviewComponent.h` | Decimates `PcmWaveform` from `RomBank` or `CleanroomWaveLibrary` into a painted path (ROM browser). |

## Editor responsibilities

- Binds APVTS sliders/combos to tone, FX, envelope, and coupling controls.
- **ROM browser** — selects tone A–D, edits `toneNWave`, shows preview; does not load external files.
- **Preset Export/Import** — `.jdpreset` XML via `JDUpgradedAudioProcessor::exportApvtsPresetToFile` / `importApvtsPresetFromFile`.

See [UI.md](../../docs/UI.md) for operator-facing notes.
