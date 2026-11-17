# MpcObXa — architecture (APC Plan)

## Program chain

```text
per voice:
  osc1 (saw) + osc2 (saw, detune_spread)
  → SVF (mode, cutoff, res)
  → ADSR
  → pan by voice index * pan_spread → stereo accumulator
```

## Components

- `ObXaVoice` in `Source/Dsp/`
- Optional `shared/dsp` biquad backup if TPT too heavy on profile — decision in Design.

## Surface

Headless; programs: Init, Wide Pad, Sync Sweep, Hollow Bass.