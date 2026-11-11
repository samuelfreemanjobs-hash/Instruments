# TRAP LORD SYNTH — Architecture

## Purpose

Sixteen-voice hybrid synthesizer for trap production: DSP engines, optional WAV multisamples, chord layers, arpeggiator, delay/reverb. Shipped as VST3 and Standalone for Disklordz Audio.

## Build and run

| Target | CMake target | Output |
|--------|--------------|--------|
| VST3 | `TrapLordSynth_VST3` | `build/TrapLordSynth_artefacts/Release/VST3/TRAP LORD SYNTH.vst3` |
| Standalone | `TrapLordSynth_Standalone` | `build/TrapLordSynth_artefacts/Release/Standalone/` |
| Headless CLI | `TrapLordSynthRenderCLI` | `build/TrapLordSynthRenderCLI_artefacts/Release/` — auditions + MPC export offline |

JUCE 8+ via `FetchContent` in `CMakeLists.txt` (tag from `JUCE_VERSION` at repo root, floor 8.0.15).

## Data flow

```
MIDI in → generateArp (chord/arp) → juce::Synthesiser (TrapVoice×16) → delay → reverb → tanh limiter → stereo out
```

Parameter changes flow through `AudioProcessorValueTreeState` on the audio thread (atomic raw parameter reads only in `processBlock`).

## Realtime constraints

- `processBlock`: no allocation, no locks, no file I/O.
- Sample bank updates: `loadSampleMap` on message thread; audio thread reads `std::shared_ptr` snapshot per block.
- Offline `exportKeygroup` calls `suspendProcessing(true)` before rendering.

## Key modules

| Path | Role |
|------|------|
| `Source/PluginProcessor.*` | APVTS, arp/chord MIDI, FX chain, preset/state, sample map load, keygroup export |
| `Source/SynthVoice.h` / `SynthVoice.cpp` | `TrapVoice` DSP: 24 synthesis engines, SVF/ladder/comb/formant filters, unison, FM, wavetable morph (synth-only factory bank) |
| `Source/PluginEditor.*` | Rotary UI, factory patch picker, sample map load/export buttons |

## Extension points

- New engines: extend `TrapVoice::renderNextBlock` switch and `engine` parameter choices.
- Presets: expand `factoryNames()` / `loadFactoryPreset()` or add program indices in processor.
- Arp sync: `arpPeriodSeconds()` uses host BPM when `getPlayHead()` provides it; otherwise 120 BPM default.

## Related docs

- `docs/ROADMAP.md` — M1–M5 claims vs gaps
- `docs/TEST_PLAN.md` — manual host checklist
