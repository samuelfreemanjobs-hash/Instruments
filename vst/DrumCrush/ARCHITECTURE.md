# DiskLordz Drum-Crush — ARCHITECTURE

## Purpose

Stereo drum bus effect: parallel transient punch, harmonic crush, brickwall soft clip, and peak limiting for hip-hop/trap (KNOCK-style macro UX).

## Build & run

| Target | Command |
|--------|---------|
| VST3 | `cmake --build build --target DrumCrush_VST3` |
| Tests | `ctest --test-dir build -R DrumCrushProcessorContracts` |

JUCE **8.0.6** at `vst/JUCE` (local clone). CMake project root: `vst/DrumCrush/`.

## Data flow

```
Input buffer
  ├─ dry copy ──────────────────────────────┐
  └─ wet: transient PUNCH + HF shelf        │
         → 4× OS CRUSH                     │
         → CLIP (rational knee)             │
         → juce::dsp::Limiter               ┘
      MIX (dry/wet) → OUTPUT gain → ±kCeiling (-0.5 dBFS)
```

## Threading / realtime

- No heap allocation in `processBlock` / `DrumCrushDsp::process` (buffers sized in `prepare`).
- `DspParamSmooth` from `shared/dsp/parameter_smoothing.h` for APVTS targets.
- `juce::ScopedNoDenormals` in processor.
- Oversampling latency exposed via `setLatencySamples`.

## Key modules

| Path | Responsibility |
|------|----------------|
| `Source/PluginProcessor.*` | APVTS, presets, state XML, bus layout |
| `Source/DrumCrushDsp.*` | Program chain (punch, crush, clip, limit, mix, ceiling) |
| `Source/PluginEditor.*` | Custom 4-knob + output UI, preset combo |
| `Source/FactoryPresets.h` | 20 factory programs |
| `tests/ProcessorTests.cpp` | Offline contracts |

## Extension points

- Add preset rows in `FactoryPresets.h`.
- Tune curves in `DrumCrushDsp::saturateCrush` / `applySoftClip`.
- CI: `.github/workflows/drumcrush.yml`.

## Related docs

- [README.md](README.md) · [BUILD_WINDOWS.md](BUILD_WINDOWS.md)
- Monorepo: `ARCHITECTURE.md`, `shared/dsp/README.md`
