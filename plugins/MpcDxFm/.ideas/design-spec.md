# MpcDxFm — design spec (v1)

**Headless** 4-op FM synth. **K1–K3:** brightness · feedback · algorithm.

## DSP

- 12-voice pool (`kMaxVoices = 12`), global ADSR, no filter.
- `FmAlgorithms4.h`: 8 routings; op1 feedback from previous sample.
- Sine operators; optional `tanh` on voice sum.
- LFO → pitch (v1 shallow, default off).

## Programs

Init, EPiano, Bell, Slap Bass, G Lead, FM Pad — `FactoryPrograms.h`.

## Melodee

`params.json` macro order matches parameter-spec.md #1–3.

## Out of scope v1

6-op mode, per-op ADSR, Nuked-OPN2 chip path (CMake flag later).