# Junova-X — architecture

## Purpose

**Junova-X** is a Juno-class analog poly synth plugin (`JunovaX.vst3` / `JunovaX.clap`) for Disklordz. MVP: JUCE port from iPlug2 reference, VST3+CLAP, 48 factory presets (WO-2026-003).

## Build & run

From repo root:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target JunovaX_VST3 JunovaX_Standalone JunovaX_CLAP
./build/Junova-X/JunovaX_artefacts/Release/Standalone/Junova-X
```

Plugin IDs: manufacturer `SmFr`, code `JnvX`.

## Data flow

```text
MIDI → SynthEngine (voices + filter/chorus stub) → master gain → outputs
         ↑ APVTS parameters (dual ADSR, HPF, chorus, diag test tone)
UI: MainPanel / DiagPanel ←→ APVTS attachments
```

## Threading / realtime

- No heap allocation in `processBlock`.
- DSP lives under `Source/DSP/`; UI only on message thread.

## Key modules

| Path | Role |
|------|------|
| `Source/PluginProcessor.*` | APVTS, state, MIDI→engine |
| `Source/DSP/SynthEngine.*` | Render + panic |
| `Source/DSP/DiagTone.*` | Sine test tone (Diag) |
| `Source/UI/MainPanel.*` | Main GUI shell (design handoff) |
| `Source/UI/DiagPanel.*` | Test tone + Panic |
| `Source/UI/UiLayout.h` | Design dimensions (Hermes GUI seat) |
| `Source/Parameters/ParameterIds.h` | Stable parameter IDs |

## Extension points

- Replace stub voice/DSP in `SynthEngine` when iPlug2 parity lands.
- Drop assets in `Resources/` and wire in `MainPanel` (see `docs/UI_DESIGN_HANDOFF.md`).
- Presets: add `Source/Presets/` in WO-2026-003.

## Related docs

- [REPO_HANDOFF.md](REPO_HANDOFF.md)
- [docs/junova-x-mvp-gap-analysis.md](docs/junova-x-mvp-gap-analysis.md)
- [docs/HERMES_AGENT_FRAMEWORK.md](../docs/HERMES_AGENT_FRAMEWORK.md)
