# DiskLordz 808-Punisher — DSP architecture (Phase 1)

## Product

Stereo insert effect: harmonic sub-bass enhancer / phone-speaker 808 saturator.

## Signal path

```text
Input
  → Linkwitz-Riley 4th-order crossover @ SUB CROSSOVER (40–120 Hz, default 60 Hz)
      Low band  ─────────────────────────────────────────────┐
      High band ──────────────────────────────── (dry mids) ┤
      Low band → 4× polyphase IIR oversampling → waveshaper ┘
          (blend: 2nd-harmonic warmth ↔ 3rd-harmonic bite via HARMONIC SHAPE)
          → subtract fundamental → harmonic layer only
  → Sum (low + high + harmonics) → MIX dry/wet → OUTPUT gain → ± ceiling (-0.5 dBFS)
```

## Design rationale

Phone and laptop speakers roll off below ~100 Hz. The LR split keeps the sub fundamental phase-coherent while a separate harmonic path adds controlled energy in the ~120–240 Hz region (2f–3f of typical 808 fundamentals). Oversampling reduces aliasing from the nonlinear stage; latency is reported to the host via `setLatencySamples()`.

## Parameters

| ID | UI | Range | Default |
|----|-----|-------|---------|
| `punish` | PUNISH | 0–100 % | 55 |
| `harmonic_shape` | HARMONIC SHAPE | 0–100 | 70 |
| `sub_crossover` | SUB CROSSOVER | 40–120 Hz | 60 |
| `mix` | MIX | 0–100 % | 100 |
| `output_gain` | OUTPUT | −12…+12 dB | 0 |

## Factory programs

20 presets in `vst/Punisher808/Source/FactoryPresets.h`: 5× UK Drill Slides, 5× Phonk Memphis Grit, 5× Trap Mobile Boost, 5× Clean Sub Tamer.

## Real-time safety

All buffers and oversampling state are prepared in `prepareToPlay`; `processBlock` has no allocations, locks, or I/O.
