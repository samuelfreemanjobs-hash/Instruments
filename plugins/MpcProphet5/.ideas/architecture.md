# MpcProphet5 — architecture (APC Plan)

## Core components

| Component | Role |
|-----------|------|
| `ProphetVoice` | 2 oscillators, sync, mixer, 4-pole LP, filter + amp ADSR |
| `PolyModRouter` | Maps osc B output (or env) to filter cutoff / osc A pitch offsets (per block) |
| `VoiceManager` | Fixed 6 voices |

## Program chain

```text
MIDI note on → ProphetVoice.start
per voice:
  oscA (saw/pulse) + oscB (saw/tri) with sync(oscB resets from oscA phase)
  mix → 4-pole LP (cutoff + filterEnv*amt + polymod)
  × ampADSR → bus
sum → stereo (dual-mono or narrow pan by voice index)
```

**Smoothing:** cutoff, resonance, polymod amounts — 15–25 ms one-pole on audio thread targets.

## Surface chain

Headless APVTS; programs: Init, Brass, Sync Lead, Bass, Fifth Pad.