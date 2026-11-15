# MpcCs80 — design spec (v1)

**v1 scope:** Single **`Cs80Layer`** per voice (Layer II mixed via `layerBalance` duplicate timbre). **4 voices** on aarch64.

## Melodee K1–K3

LPF **cutoff** · **resonance** · **layerBalance** (wet mix toward Layer II path)

## DSP

- Morph saw/tri/square + PWM on VCO1; detuned VCO2; sub square; optional ring (fixed low mix).
- **HPF → LPF** `StateVariableTPTFilter` 12 dB, filter + amp ADSR.
- Poly aftertouch → +cutoff on held voice (MIDI poly pressure).

## Programs (8)

Brass Init, Blade Sweep, String Pad, AT Lead, Soft Brass, Res Sweep, Sub Brass, Film Pad.

## v2

True independent Layer I/II timbres, 6–8 voices, chorus.