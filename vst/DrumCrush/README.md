# DiskLordz Drum-Crush

Phase 2 commercial plugin — **Parallel Bus Transient Punch & Brickwall Soft-Clipper** (hip-hop / trap drum bus; KNOCK-class macro workflow).

## Controls (4 macros + output)

| Knob | Role |
|------|------|
| **PUNCH** | Dual-rate envelope transient emphasis + coupled 4.2 kHz high-shelf “snap” |
| **CRUSH** | Parallel-path harmonic saturation (4× polyphase IIR oversampling anti-alias) |
| **CLIP** | Brickwall-soft rational knee before the peak limiter |
| **MIX** | Dry/wet parallel bus (dry = unprocessed input) |
| **OUTPUT** | Post-mix trim (dB); final stage always enforces **-0.5 dBFS** ceiling |

## Build (Linux)

```bash
git clone --branch 8.0.6 --depth 1 https://github.com/juce-framework/JUCE.git vst/JUCE
cd vst/DrumCrush
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12
cmake --build build -j --target DrumCrush_VST3 DrumCrushProcessorTests
ctest --test-dir build -R DrumCrushProcessorContracts --output-on-failure
```

Windows: see [BUILD_WINDOWS.md](BUILD_WINDOWS.md).

## Product notes

- APVTS parameters with `DspParamSmooth` de-zippering (`shared/dsp/parameter_smoothing.h`).
- Oversampling latency reported via `AudioProcessor::setLatencySamples`.
- 20 factory presets (trap / drill / parallel / clip categories).
- **Not DAW-validated** in cloud CI — Windows workflow runs pluginval strictness 5 only.
