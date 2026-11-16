---
name: drum-synth-dsp
description: Scaffolds real-time-safe JUCE drum voices (808/kick, snare, clap, hi-hat) and points at the shared drum engine. Use when adding or editing a drum synthesizer voice.
---

# JUCE drum synthesis

When asked to create or modify a drum synthesis voice, follow these rules.

## 1. Core voice requirements

- Never allocate, resize a vector, or lock a mutex inside `renderNextBlock()` or `process()`. Pre-allocate in `prepare(const juce::dsp::ProcessSpec&)`.
- Place `juce::ScopedNoDenormals` at the entry of every audio function.
- On `noteOn()` / `trigger()`, set oscillator phase to 0 unless free-running drift was requested.
- Smooth continuous parameters with `juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>`.

## 2. Drum topology

- 808 / kick: `double` phase accumulator, `std::sin(phase * twoPi)`, exponential pitch envelope `base + sweep * exp(-t / pitchDecay)`, a click under 2 ms, and `std::tanh` inside `juce::dsp::Oversampling<float>`.
- Snare: fundamental plus about 1.8x overtone, plus noise through `juce::dsp::StateVariableTPTFilter` bandpass.
- Clap: 3 to 4 bursts spaced 8–15 ms, then a noise tail.
- Hi-hat: six squares at ratios `1.0, 1.483, 1.932, 2.546, 2.630, 3.897`, then resonant bandpass and a highpass. Generate the squares at the oversampled rate.

## 3. Where the code goes

The shared engine is `disklordz/drum-engine/include/`, not `Source/DSP/` (that tree is JD Upgraded). Both `DrumEngineTests` and `DrumBatchExporter` compile the same headers. DSP headers include `juce_dsp` / `juce_audio_basics` only.

On JUCE 8.0.15, `Oversampling`'s factor argument is an exponent. The repo uses `2`, which is 4x. Do not "correct" it to `4` (that is 16x).

Reference voices: `Kick808Voice.h`, `SnareVoice.h`, `ClapVoice.h`, `HiHatVoice.h`. Register a new voice in `MidiMaps.h`.

## 4. Prove it

```bash
cmake -S disklordz/drum-engine -B disklordz/drum-engine/build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build disklordz/drum-engine/build --target DrumEngineTests
ctest --test-dir disklordz/drum-engine/build --output-on-failure
```

The test checks identical hits after a phase reset, finite output, a settled tail, a scratch buffer that does not grow, and 4x oversampling.
