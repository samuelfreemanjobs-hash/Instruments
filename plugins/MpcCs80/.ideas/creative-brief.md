# MpcCs80 — creative brief

**Target:** Yamaha **CS-80** character for Melodee / Linux VST3: **Blade Runner brass**, **sweeping dual filters**, **string pads**, **polyphonic aftertouch** leads. Not a cycle-accurate clone — a **dual-layer expressive polysynth** that fits MPC Sample CPU.

## Sonic pillars

1. **Dual layers (I / II)** — two complete timbre stacks per key; layer balance and split for brass vs. strings.
2. **Two VCOs per layer** — saw / pulse with **PWM**; detune and mix for width.
3. **Sub + ring mod** — square sub (½ freq) and **ring modulator** (VCO1 × VCO2) per layer for CS-family grit and harmonics.
4. **HPF → LPF** — independent high-pass and low-pass (12 dB/oct v1) with shared envelope modulation; signature **filter sweeps**.
5. **Poly aftertouch** — per-note pressure → filter cutoff / level (v1: MIDI channel pressure poly AT only).

## Scope v1 (MPC-safe)

- **4 voices** (each voice = Layer I + Layer II path, not 16 unison).
- Headless Melodee; **K1–K3** = LPF cutoff, resonance, layer I/II balance.
- 12–16 factory programs: *Brass Sweep*, *Tears Strings*, *Aftertouch Lead*, *Soft Pad*.
- v2: ribbon / channel pressure curves, chorus ensemble, 6–8 voices if profiling allows.

## Reference listening

- Blade Runner title brass (layered filter movement)
- CS-80 string ensembles with slow filter opening
- Expressive mono-line leads with aftertouch opening the filter