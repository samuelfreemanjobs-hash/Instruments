---
name: disklordz-cpp-realtime
description: Realtime-safe C++20 for audio DSP — memory, SIMD, control rate, determinism, and Instruments voice/DSP patterns.
when_to_use: Write or optimize DSP inner loops, voice rendering, envelopes, filters, buffers, or any code called from processBlock.
disable-model-invocation: false
context: fork
paths:
  - "Source/DSP/**"
  - "Wave9090/**"
  - "SP1200/**"
  - "tools/OfflineRender.cpp"
  - "tests/**"
---

# Skill — C++ realtime audio coding

## Hard rules (audio thread)

1. **No blocking** — no mutex locks, condition variables, file I/O, network, logging, or unbounded containers.
2. **No unbounded allocation** — no `new`/`delete`, `std::vector::push_back`, `std::string` growth, or `juce::String` concat in `processBlock` / `render`.
3. **No exceptions** — mark hot paths `noexcept` where practical; do not throw across plugin boundary.
4. **Fixed upper bounds** — max voices, max block size known from `prepareToPlay`; size scratch once there.
5. **Determinism** — same MIDI + params → same output (CI uses offline render + golden WAV / SpectralDiff).

When you need dynamic structure, allocate on **prepareToPlay** or message thread, then use fixed storage in audio.

## Control rate vs sample rate

This repo decimates control-rate work (example: **`JDUPGRADED_CONTROL_RATE_DIVISOR=32`** in root `CMakeLists.txt`):

- Envelopes / LFOs may advance every N samples inside `Tone` — match existing divisor unless product ARCHITECTURE says otherwise.
- Filter coefficients: update at control rate when cutoff is param-driven; interpolate if you must change every sample.

Pattern:

```cpp
for (int i = 0; i < numSamples; ++i)
{
    if ((controlCounter++ % kControlRateDivisor) == 0)
        envelope.tick();
    sample = oscillator.next() * envelope.current();
    out[i] = sample;
}
```

Read the product’s voice/tone code before inventing a new divisor.

## Memory & layout

- Prefer **`std::array`** for small fixed tables (per-voice state).
- Scratch buffers: member `juce::AudioBuffer<float>` or `std::vector<float>` sized in `prepareToPlay(maxBlock)`.
- Align SIMD loads: use JUCE `FloatVectorOperations` or project `VoiceSimd` helpers ([Source/DSP/Simd/VoiceSimd.h](../../../../../Source/DSP/Simd/VoiceSimd.h)).
- Avoid `std::function` on audio path — use templates or function pointers known at compile time.

## SIMD (Instruments convention)

Root CMake probes **AVX2** (`-mavx2 -mfma`) and defines **`JDUPGRADED_USE_AVX2`**, else **NEON** on ARM (`JDUPGRADED_USE_NEON`).

- Wrap intrinsics in `#if JDUPGRADED_USE_AVX2` / `#elif JDUPGRADED_USE_NEON` with scalar fallback.
- Never assume AVX2 on CI ARM runners — scalar path must compile and sound correct.
- Voice accumulation: see coupled-tone SIMD add in [docs/ARCHITECTURE.md](../../../../docs/ARCHITECTURE.md).

## Denormals & FP hygiene

At `processBlock` entry:

```cpp
juce::ScopedNoDenormals noDenormals;
```

For long IIR chains, consider FTZ/DAZ at `prepareToPlay` if platform allows — document if you do.

## Parameters on audio thread

- Read APVTS via **raw atomic pointers** (`getRawParameterValue`) or cached `float` updated in parameter listener on message thread.
- Do not call `getValue()` on parameters that allocate internally in hot loops.
- Smooth stepped parameters (MIDI-style) with `SmoothedValue` or one-pole smoother.

## MIDI & voice management

- Allocate/release voices in pool on message or audio thread per existing `VoicePool` design — no `std::map` insert per note on audio thread if avoidable.
- Use `noexcept` on `render` methods; return silence on invalid state rather than throwing.

## Testing strategy

| Level | Tool |
|-------|------|
| Unit | `ctest` targets (`Wave9090Tests`, `SP1200*Tests`, etc.) |
| Determinism | `OfflineRender` + `SpectralDiff` |
| Regression | `tests/golden/verify_golden.sh` |
| Host | pluginval via `vst-testing-ops` |

After intentional timbre change: `tests/golden/refresh_golden.sh` + update `manifest.tsv`.

## Code style (match monorepo)

- C++ **20**; `final` on processor classes where sealed.
- Namespace product DSP under `jdupgraded::dsp` (or product-specific namespace — follow sibling files).
- Headers in `Source/DSP/` — minimal includes in headers; forward declare when possible.
- Keep `.cpp` for heavy templates if compile time hurts.

## Anti-patterns

- `DBG()` spam in release audio paths (use gated debug or offline-only traces).
- Global static mutable state without audio-safe initialization.
- Double precision “for quality” on whole path without profiling need — default **float** for realtime.
- Widening golden diff thresholds to greenwash failures.

## Verify

```bash
cmake --build build -j --target OfflineRender SpectralDiff
python3 vst-testing-ops/run_business.py --profile dsp-only
ctest --test-dir build --output-on-failure
```

## Related skills

- **`disklordz-juce-coding`** — APVTS, CMake, formats
- **`disklordz-dsp-vst-architect`** — end-to-end factory agent
