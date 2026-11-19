# Plugg VA — Product Specification

## Vision

**Plugg VA** is a **virtual-analog** VSTi for trap, plugg, phonk, rage, and Southern street production. It targets the harmonic language of 2000s–2020s Atlanta and Memphis beat culture: detuned dual oscillators, portamento leads, resonant plucks, saturated subs, and wide unison — without a sample library.

Reference *styles* (not artist endorsements): Pierre-style plucks, Carti space leads, Gucci-era supersaws, Shawty Redd glide chords, Sledgren arena bells, phonk subs, rage stabs, “majestic casual” wide pads.

## P0 feature set (v0.1)

| Block | Capability |
|-------|------------|
| Oscillators | OSC1 + OSC2: saw, square, pulse, triangle (polyBLEP) |
| Sub | Sine one octave below |
| Unison | 1–5 stack, spread in cents |
| Filter | 2-pole ZDF lowpass, cutoff + resonance, filter envelope |
| Amp | ADSR |
| Filter env | ADSR → cutoff |
| Motion | Glide, chorus (global), drift/wow macro |
| Grit | Soft drive + optional gentle crush |
| Voices | 16 poly, steal oldest |
| Presets | 12 factory programs named by subgenre workflow |
| UI | Dark trap panel (rotary macros) |

## Companion in monorepo

- **WAVE-9090** — wavetable / sampleless hybrid grit (phonk macros).
- **Plugg VA** — classic VA stack for “synth plugg” and supersaw trap.

## Non-goals (v0.1)

- MPE, user wavetables, full mod matrix, AU/CLAP, preset browser beyond factory list.

## Build

From repo root:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target PluggVA_VST3 PluggVA_Standalone PluggVATests
ctest --test-dir build -R PluggVA --output-on-failure
```
