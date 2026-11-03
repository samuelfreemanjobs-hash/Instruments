---
name: dsp-program-chain
description: Implements the audio program chain (voices, oscillators, filters, FX, processBlock/OSC_CYCLE) per DSP architecture spec §4. Use when writing or refactoring realtime DSP, voice management, or audio callback code.
---

# DSP Program Chain subagent

## Read first

`docs/dsp-architecture-specification.md` — **§4 Program chain**, **§3 Core components**

## Scope

**In scope**

- `vst/<Plugin>/Source/Dsp/**`
- `vst/<Plugin>/Source/PluginProcessor.cpp` — `prepareToPlay`, `processBlock` only
- `src/oscillators/**/*.cc` — `OSC_CYCLE`
- `src/mkii/oscillators/**/osc.h` — `process()`
- `shared/dsp/**` — portable algorithms

**Out of scope**

- `PluginEditor.*` (surface chain)
- CMake, manifest (other skills)

## Implementation rules

1. **Order** — Follow canonical synth or effect order from spec §4.2 / §4.3.
2. **Allocation** — Preallocate in `prepareToPlay` / `init`; zero alloc in audio callback.
3. **MIDI** — Voice manager owns note state; do not rely on MIDI only in current buffer for held notes.
4. **Includes** — Dsp headers must not include JUCE GUI modules.
5. **logue** — Prefer float internally on mkII; q31 at boundary on v1.1.

## Voice manager template (L1+)

- Fixed pool `kMaxVoices` (e.g. 8).
- Steal oldest active voice on overflow.
- Per voice: phase, envelope phase, filter state.

## Deliverables

- List program stages in plugin `README.md` (bullet order).
- Comment `// PROGRAM:` stage markers in `processBlock` for long functions.
- Unit-testable pure functions in `shared/dsp/` when possible.

## Checklist before done

- [ ] No locks/alloc/I/O in audio callback
- [ ] `ScopedNoDenormals` (JUCE) where applicable
- [ ] Parameters read via smoothed values, not raw APVTS in inner loop without smoothing
- [ ] Complexity tier matches implementation (no 16-voice design at L0)
