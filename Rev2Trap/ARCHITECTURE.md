# Rev2 Trap — Prophet-vibe trap synthesizer

## Purpose

VST3 + Standalone **Rev2 Trap**: dual-osc + **ladder filter** synth tuned for **original trap** timbres (not song clones). Primary **multisample generator** for product **REV2-TRAP-128** via `Rev2TrapOfflineRender`.

## Build & run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target Rev2Trap_VST3 Rev2Trap_Standalone Rev2TrapOfflineRender Rev2TrapTests
ctest -R Rev2TrapSmoke
```

Offline sample:

```bash
./build/Rev2Trap/Rev2TrapOfflineRender /tmp/note.wav 0 60 100 2.8 44100
```

## Data flow

MIDI → `Rev2TrapAudioProcessor` → 16× `Rev2Voice` (saw/square + sub → ladder LPF → ADSR) → stereo out.

**528 factory programs:** 0–127 Rev2 trap (`FactoryPresets.cpp`), 128–527 JZ400 bank (`Jz400Presets.cpp`). DSP: wavetable blend, hard sync, filter FM, drift, trap macro.

## Threading

Standard JUCE: no allocation on audio thread in `processBlock`.

## Key modules

| Path | Role |
|------|------|
| `Source/DSP/Rev2Voice.h` | Prophet-style voice |
| `Source/Presets/FactoryPresets.*` | 128 programs |
| `tools/Rev2TrapOfflineRender.cpp` | Headless WAV for factory |

## Extension points

- Poly/unison, wavetable osc, FX (chorus/dist), CLAP format
- Deeper Rev2: sync, wavetable blend (see improvement doc)

## Related

- [docs/products/REV2_TRAP128.md](../docs/products/REV2_TRAP128.md)
- [docs/FACTORY_IMPROVEMENT_ROADMAP.md](../docs/FACTORY_IMPROVEMENT_ROADMAP.md)
