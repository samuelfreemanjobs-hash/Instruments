# Instruments

This monorepo hosts **JD Upgraded** (Roland JD-style synth), **SP-1200 Drumulator** ([SP1200/](SP1200/) standalone + VST3 sampler), **NTS-1 multi-bass** custom oscillators ([docs/nts1-multi-bass-oscillators.md](docs/nts1-multi-bass-oscillators.md)), and the **MyFirstPlugin** JUCE starter templates.

---

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
| `VstiOneShotRender` | Any VST3 → one-shot WAVs (see [VSTi one-shot handoff](docs/VSTI_ONE_SHOT_HANDOFF.md)) |

## VSTi one-shots for MPC (Boom)

Style brief → **100 mono WAV one-shots** from any VST3 factory bank:

**Windows:**

```powershell
.\scripts\boom.ps1 -Style "dark melodic stabs" -Vst3 "C:\Program Files\Common Files\VST3\Serum.vst3" -Out "$env:USERPROFILE\Exports\pack"
```

**Linux / macOS:** `./scripts/boom.sh --style "..." --vst3 "..." --out ~/Exports/pack`

See [docs/VSTI_ONE_SHOT_WINDOWS.md](docs/VSTI_ONE_SHOT_WINDOWS.md) · [docs/VSTI_ONE_SHOT_HANDOFF.md](docs/VSTI_ONE_SHOT_HANDOFF.md).

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

---

# NTS-1 multi-bass (logue SDK)

Genre **bass and kick user oscillators** for **NTS-1 mkI** and **NTS-1 mkII** (`.ntkdigunit` / `.nts1mkiiunit`). One custom osc loaded at a time.

| Unit folder | Panel name |
|-------------|------------|
| `tr808-kick` | Phonk 808 |
| `west-coast-moog` | P-Funk WC |
| `juno-rnb` | Juno R&B |
| `dilla-bass` | Dilla Bass |
| `sub-phatty` | Sub Phatty |
| `prophet-funk` | P5 Funk RB |
| `dx7-lately` | Lately Bass |
| `sh101-babyface` | SH-101 BF |
| `moog-voyager-se` | Voyager SE |
| `cardo-gfunk` | Cardo GF |
| `larry-june` | Larry June |
| `khalifa-haze` | Khalifa Haze |
| `memphis-juicy` | Juicy 36 |
| `ensoniq-eps1` | EPS-1 MEM |

```bash
./tools/mkii-automate.sh bootstrap
export LOGUE_SDK="$(pwd)/.deps/logue-sdk"
./tools/build-all-bass-mkii.sh    # mkII
./tools/build-all-bass.sh         # mkI
./tools/package-nts1.sh
```

Install: [docs/NTS1_INSTALL_TODAY.md](docs/NTS1_INSTALL_TODAY.md). mkII porting notes: [docs/nts1-mkii-porting.md](docs/nts1-mkii-porting.md).

Optional **VST/APC** workflow for new JUCE plugins: [vst/README.md](vst/README.md), [docs/apc-workflow.md](docs/apc-workflow.md).

---

# MyFirstPlugin (starter)

CMake-native [JUCE](https://juce.com/) effect and synth templates under [`MyFirstPlugin/`](MyFirstPlugin/). JUCE version is pinned in [`JUCE_VERSION`](JUCE_VERSION) (fetched on first configure for that subproject).

**Windows:** `MyFirstPlugin/Build Plugin.bat` or see [`MyFirstPlugin/VISUAL_STUDIO.md`](MyFirstPlugin/VISUAL_STUDIO.md).  
**macOS / Linux:** `cd MyFirstPlugin && ./build.sh`

CI for the starter: [`.github/workflows/build-plugin.yml`](.github/workflows/build-plugin.yml).
