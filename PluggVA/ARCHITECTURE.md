# Plugg VA — architecture

Virtual-analog trap/plugg synthesizer (VST3 + Standalone). Repo index: [ARCHITECTURE.md](../ARCHITECTURE.md).

## Purpose

Dual polyBLEP oscillators, sub, unison, ZDF lowpass, glide, chorus, and drive — tuned for Atlanta plugg, rage, phonk, and Southern lead workflows.

## Build and run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target PluggVA_VST3 PluggVA_Standalone PluggVATests
```

Artifact: `build/PluggVA/PluggVA_artefacts/Release/VST3/Plugg VA.vst3`

## Data flow

MIDI → `PluggVAAudioProcessor` → `SynthEngine` → `VoiceManager` → per-voice VA DSP → stereo chorus → output gain.

Parameters: JUCE APVTS (`Parameters/ParameterIds.h`).

## Threading

| Thread | Work |
|--------|------|
| Audio | `processBlock`, voice render, no allocation |
| Message | Editor, preset changes |

## Key modules

| Path | Responsibility |
|------|----------------|
| `Source/PluginProcessor.*` | Host glue, APVTS, programs |
| `Source/Engine/SynthEngine.*` | Engine facade |
| `Source/Engine/VoiceManager.*` | Voice allocation |
| `Source/Engine/Voice.*` | Per-voice VA chain |
| `Source/DSP/PolyBlepOscillator.h` | Band-limited analog waves |
| `Source/DSP/ZdfLowpass.h` | Ladder-style LP |
| `Source/Presets/FactoryPresets.*` | Genre factory programs |
| `Source/UI/PluginEditor.*` | Dark editor |

## Related docs

- [Docs/PRODUCT_SPEC.md](Docs/PRODUCT_SPEC.md)
- [Wave9090/ARCHITECTURE.md](../Wave9090/ARCHITECTURE.md) — wavetable sibling
