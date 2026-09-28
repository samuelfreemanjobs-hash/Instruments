# Junova-X — architecture

## Purpose

**Junova-X** is a Juno-class analog poly synth plugin (`JunovaX.vst3` / `JunovaX.clap`) for Disklordz. MVP: JUCE VST3+CLAP+Standalone, Celestial UI, analog-style poly DSP, **48 factory presets** (WO-2026-003). iPlug2 remains reference for future parity.

**Phase 2:** modular DSP, host-aware arpeggiator — see [docs/ROADMAP.md](docs/ROADMAP.md) and [docs/ARCHITECTURE_DSP.md](docs/ARCHITECTURE_DSP.md).

## Build & run

From repo root:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target JunovaX_VST3 JunovaX_Standalone JunovaX_CLAP JunovaXTests
ctest -R JunovaXArpeggiator --test-dir build --output-on-failure
./build/Junova-X/JunovaX_artefacts/Release/Standalone/Junova-X
```

Plugin IDs: manufacturer `SmFr`, code `JnvX`.

## Data flow

```text
MIDI → Arpeggiator (optional, BPM) → SynthEngine (8-voice poly) → master → outputs
         ↑ APVTS                              ↑ ScopeFifo → OSC UI
UI: CelestialMainPanel / DiagPanel ←→ APVTS attachments
```

## Threading / realtime

- No heap allocation in `processBlock`.
- DSP lives under `Source/DSP/`; UI only on message thread.

## Key modules

| Path | Role |
|------|------|
| `Source/PluginProcessor.*` | APVTS, arp, presets, state |
| `Source/DSP/Arpeggiator.*` | Pre-voice MIDI arp |
| `Source/DSP/SynthEngine.*` | Voices, VCF, routing |
| `Source/DSP/BbdChorus.*` | Stereo chorus |
| `Source/DSP/DiagTone.*` | Sine test tone (Diag) |
| `Source/DSP/ScopeFifo.h` | OSC monitor samples |
| `Source/UI/Celestial/*` | Main Celestial GUI |
| `Source/UI/DiagPanel.*` | Test tone + Panic |
| `Source/UI/UiLayout.h` | Design dimensions |
| `Source/Parameters/ParameterIds.h` | Stable parameter IDs |
| `Source/Presets/FactoryPresets.cpp` | 48 factory programs |
| `Tests/DSP/ArpeggiatorTests.cpp` | DSP unit smoke |

## Extension points

- New DSP blocks: add under `Source/DSP/`, wire from `SynthEngine` or processor; document in [ARCHITECTURE_DSP.md](docs/ARCHITECTURE_DSP.md).
- iPlug2 parity: follow parity table in ARCHITECTURE_DSP — no JD Upgraded kernel sharing.
- Presets: edit `scripts/generate_factory_presets.py` or add bank 2 files under `Source/Presets/`.
- UI assets: `Resources/` + [docs/UI_DESIGN_HANDOFF.md](docs/UI_DESIGN_HANDOFF.md).

## Related docs

- [REPO_HANDOFF.md](REPO_HANDOFF.md)
- [docs/junova-x-mvp-gap-analysis.md](docs/junova-x-mvp-gap-analysis.md)
- [docs/CLAP_CI_CHECKLIST.md](docs/CLAP_CI_CHECKLIST.md)
- [docs/QA_HOST_SMOKE.md](docs/QA_HOST_SMOKE.md)
- [docs/HERMES_AGENT_FRAMEWORK.md](../docs/HERMES_AGENT_FRAMEWORK.md)
