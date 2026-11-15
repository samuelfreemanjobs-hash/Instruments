# DiskLordz Phonk-Tape

Memphis cassette coloration and high-speed wow/flutter for drift phonk, Memphis rap, and lofi.

## Controls

| Parameter | Role |
|-----------|------|
| **DRIVE** | Tape-style saturation (4× oversampled rational + tanh blend) |
| **WOW** | Slow (~0.72 Hz) LFO depth on modulated delay time |
| **FLUTTER** | Fast (~7.8 Hz) LFO depth for pitch shimmer |
| **AGE** | One-pole HF rolloff (18 kHz → 2.8 kHz) |
| **NOISE** | Band-limited hiss (LCG, no heap) |
| **MIX** | Dry/wet |
| **OUTPUT** | Trim (dB) before -0.5 dBFS ceiling |

## DSP design notes

### Wow / flutter (fractional delay)

Pitch drift is implemented as a **modulated read head** on a **preallocated per-channel ring buffer** (no allocation in `process`). Each sample:

1. Write input to `writePos`.
2. Compute delay in samples: `base + wow·sin(ω_w t) + flutter·sin(ω_f t)`.
3. Read at `readPos = writePos − delay` using **linear fractional interpolation** between wrapped indices (cheap, stable; Hermite optional later).

A fixed **base delay** (~10.5 ms) keeps the read pointer strictly in the past. **Reported latency** = base + maximum wow/flutter depth + oversampling latency so hosts can compensate.

### Tape saturation

Nonlinear stage runs at **4× polyphase IIR oversampling** (JUCE `dsp::Oversampling`) to limit aliasing on cowbell and distorted 808 transients. Curve: asymmetric rational mapper mixed with `tanh` for Memphis warmth vs grit.

### Realtime safety

- `ScopedNoDenormals` in the processor.
- Smoothed parameters via `shared/dsp/parameter_smoothing.h`.
- No locks, file I/O, or heap in `processBlock`.
- NaN/Inf sanitized; output hard-limited to **-0.5 dBFS**.

## Build (Linux)

```bash
git clone --branch 8.0.6 --depth 1 https://github.com/juce-framework/JUCE.git vst/JUCE
cd vst/PhonkTape
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12
cmake --build build -j --target PhonkTape_VST3 PhonkTapeProcessorTests
ctest --test-dir build -R PhonkTapeProcessorContracts --output-on-failure
```

Windows: see [BUILD_WINDOWS.md](BUILD_WINDOWS.md).

## Factory presets

20 presets (drift phonk, Memphis grit, lofi, mix utilities) via the host program list / UI preset combo.
