# AFTRDARK — architecture

Sampleless wavetable synthesizer (VST3 + Standalone). Repo index: [ARCHITECTURE.md](../ARCHITECTURE.md).

## Purpose

Dark trap / phonk instrument: morphing algorithmic wavetables, dual filter circuit, tape wobble macro. **No embedded audio samples.**

## Build and run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target AFTRDARK_VST3 AFTRDARK_Standalone AftrdarkTests
```

Artifact: `build/AFTRDARK_artefacts/Release/VST3/AFTRDARK.vst3`

## Data flow

MIDI → `AftrdarkAudioProcessor` → `SynthEngine` → `VoiceManager` → per-voice DSP → stereo out.

Parameters: JUCE APVTS (`Parameters/ParameterIds.h`), read on audio thread via cached atomics/smoothed values.

## Threading

| Thread | Work |
|--------|------|
| Audio | `processBlock`, voice render, no allocation |
| Message | Editor, preset changes |
| Background | None in v0.1 |

## Key modules

| Path | Responsibility |
|------|----------------|
| `Source/PluginProcessor.*` | Host glue, APVTS, programs |
| `Source/Engine/SynthEngine.*` | Wavetable bank, global FX |
| `Source/Engine/VoiceManager.*` | Voice allocation |
| `Source/Engine/Voice.*` | Single voice DSP chain |
| `Source/DSP/Wavetable*.h` | Tables + oscillator |
| `Source/DSP/Filter/ZdfLowpass.h` | Pristine filter |
| `Source/DSP/Effects/DestructiveStage.h` | Drive + crush |
| `Source/DSP/Effects/TapeWobble.h` | Macro modulation |
| `Source/Presets/FactoryPresets.*` | Factory programs |
| `Source/UI/*` | Dark editor |

## Extension points

- New parameters: `ParameterIds.h`, `createParameterLayout`, `SynthEngine::refreshParameters`
- New factory preset: `FactoryPresets.cpp`
- User wavetable import: `WavetableBank` loader (P1)

## Related docs

- [Docs/PRODUCT_SPEC.md](Docs/PRODUCT_SPEC.md)
- [Docs/DSP_SPEC.md](Docs/DSP_SPEC.md)
- [Docs/PARAMETER_MAP.md](Docs/PARAMETER_MAP.md)
