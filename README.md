# Instruments — JD Upgraded, plugins, logue mkII oscillators

This monorepo hosts **JD Upgraded** (Roland JD-style synth), **MyFirstPlugin** / **vst/** JUCE templates, **Disklordz SaaS**, and **logue SDK** custom oscillators (**NTS-1 mkII** standard).

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

AU (macOS): see [docs/INTEGRATION.md](docs/INTEGRATION.md).

## Docs

- [Repository architecture index](ARCHITECTURE.md) (required reading for agents)
- [JD Upgraded architecture](docs/ARCHITECTURE.md)
- [Offline tools architecture](tools/ARCHITECTURE.md)
- [UI](docs/UI.md) · [Agent handoff](docs/HANDOFF.md)

## Environment

`JDUPGRADED_ROM_PATH` — load a validated `JDUPGROM` file instead of the embedded ROM (development).

---

# MyFirstPlugin (starter)

CMake-native [JUCE](https://juce.com/) templates under [`MyFirstPlugin/`](MyFirstPlugin/). See [`MyFirstPlugin/VISUAL_STUDIO.md`](MyFirstPlugin/VISUAL_STUDIO.md) on Windows.

---

# logue SDK — NTS-1 mkII custom oscillators (standard)

**Ship target:** NTS-1 mkII (SDK **v2**, `.nts1mkiiunit`).  
**Agent:** [docs/logue-mkii-oscillator-agent.md](docs/logue-mkii-oscillator-agent.md) · golden unit [docs/logue-mkii-golden-unit.md](docs/logue-mkii-golden-unit.md)

```bash
chmod +x tools/mkii-automate.sh tools/osc-eval-mkii.sh
./tools/mkii-automate.sh bootstrap
./tools/osc-eval-mkii.sh tr808_kick_phonk    # golden reference build
./tools/build-mkii.sh <mkii-slug>
```

Catalog (v1.1 sources + mkII ports): [docs/nts1-multi-bass-oscillators.md](docs/nts1-multi-bass-oscillators.md) · porting [docs/nts1-mkii-porting.md](docs/nts1-mkii-porting.md)

Optional **v1.1** (Minilogue XD / NTS-1 mkI / Prologue): `./tools/build.sh oscillators/<name> minilogue-xd` — [docs/platform-matrix.md](docs/platform-matrix.md).

---

# VST / APC lane

New plugins from `vst/template/`: [vst/README.md](vst/README.md) · [docs/apc-workflow.md](docs/apc-workflow.md)
