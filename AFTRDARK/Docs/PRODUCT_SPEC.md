# AFTRDARK — Product Specification

## Product vision

AFTRDARK is a **sampleless** virtual instrument for trap, phonk, and dark cinematic production. It combines wavetable morphing inspired by high-end hybrid digital-analog synths with street-ready grit: diode-style saturation, bit reduction, and cassette wobble macros.

## Target audience

Beat producers who want harmonic complexity and movement without a sample library workflow.

## Core workflow

1. Browse factory presets (bells, plucks, leads, subs).
2. Sweep wavetable position for timbral morphs.
3. Toggle filter circuit mode and drive for aggression.
4. Ride the **Tape Wobble** macro for lo-fi pitch drift and hiss.

## Core features (P0)

- Algorithmic 64-wave wavetable engine (no embedded audio samples).
- Wavetable position + scan rate modulation.
- Pristine 24 dB-style ZDF lowpass vs destructive sat/crush path.
- Amp and filter envelopes.
- Tape wobble macro (wow, flutter, noise floor, sub detune coupling).
- Sub oscillator.
- 16-voice polyphony with voice stealing.
- Factory presets with categories.
- Dark instrument UI.

## Non-goals (v0.1)

- User wavetable import (P1).
- Full modulation matrix UI (P1).
- CLAP/AU packaging (P1).
- MPE (P2).

## Sonic philosophy

Digital harmonic richness first; analog-style filtering and intentional destruction second. Presets skew dark, metallic, and unstable rather than polite ambient.

## UX philosophy

Serum-meets-ROMpler layout: macros and filter mode up front; envelopes secondary.

## Technical requirements

- C++20, JUCE 8, CMake, VST3 + Standalone.
- Real-time safe audio path.
- Versioned APVTS state.

## Performance targets

- 16 voices at 48 kHz, 512 buffer: target under 15% single core (ASSUMED until profiled).

## Release requirements

- Builds in monorepo CI when enabled.
- DSP unit tests for wavetable and filter sanity.
