# JD Upgraded

Clean-room JUCE **VST3**, **CLAP**, and **standalone** synthesizer inspired by the Roland JD-800 / JD-990: four tones per voice, PCM ROM playback, ZDF TVF, tone coupling, Group A/B effects, and 128 factory programs.

**Legal:** Sound comes from **`jdupg_cleanroom.rom`** — 256 procedurally synthesized waves ([docs/ROM.md](docs/ROM.md)). No Roland ROM ships with the project. Optional dev override: [docs/USER_ROM.md](docs/USER_ROM.md).

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j
```

| Target | Output |
|--------|--------|
| `JDUpgraded_VST3` | VST3 plugin |
| `JDUpgraded_CLAP` | CLAP plugin |
| `JDUpgraded_Standalone` | Desktop app |
| `OfflineRender` | Headless WAV render (see [docs/AB_HARNESS.md](docs/AB_HARNESS.md)) |
| `SpectralDiff` | Compare two WAVs for regression / A/B |
| `GenerateCleanroomRom` | Rebuild ROM at compile time |
| `ExportPreset` | Write `.jdpreset` for a factory program index |

AU (macOS): see [docs/INTEGRATION.md](docs/INTEGRATION.md).

## Features (current)

- 32-voice pool, 4 tones per voice, zero heap allocation on the audio thread
- 256-wave clean-room ROM with 24 multisample sets (8 zones each)
- 128 factory patches (EP, pad, bass, vapor, R&B, elite categories)
- Global and per-tone Amp/Filter ADSR, wave palette by category
- Roland JD patch SysEx import (partial APVTS mapping) — [docs/SYSEX.md](docs/SYSEX.md)
- AVX2 SIMD tone sum (independent coupling path); NEON on ARM when AVX2 is unavailable

## Docs

- [Repository architecture index](ARCHITECTURE.md) (required reading for agents)
- [JD Upgraded architecture](docs/ARCHITECTURE.md)
- [Offline tools architecture](tools/ARCHITECTURE.md)
- [UI](docs/UI.md) · [Installer policy](docs/INSTALLER_POLICY.md)
- [Presets](docs/PRESETS.md)
- [Agent handoff](docs/HANDOFF.md)
- [Phase 5 roadmap](docs/PHASE5.md)

## Environment

`JDUPGRADED_ROM_PATH` — load a validated `JDUPGROM` file instead of the embedded ROM (development).
