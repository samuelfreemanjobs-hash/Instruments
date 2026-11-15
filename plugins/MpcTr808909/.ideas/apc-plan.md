# MpcTr808909 — APC plan

**Next:** `/apc-design MpcTr808909`

## Complexity (spec §8)

| Dimension | Score | Notes |
|-----------|-------|-------|
| Voices | 2 | many one-shot overlaps, not poly synth |
| Oscillators/voice | 2 | 1–2 per drum type |
| Filters | 1 | BP snare, HPF hat |
| Modulation | 1 | kick pitch sweep |
| FX | 1 | bus drive |
| Surfaces | 1 | headless + kits |
| Targets | 1 | |
| **Total** | **9** | |

**Tier:** **L2**

**Risk flags:** aliasing on fast kick sweep — minimum sweep time floor; test at 44.1 kHz.

**Melodee upside:** Live **decay/tune/drive** on K1–K3 beats swapping static 808 WAVs.

## UI framework

`headless` — K1=decay, K2=tune, K3=drive (focused voice).

## Strategy

- `Source/Dsp/drums/Kick808.h`, `Snare808.h`, `Hat909.h`, … unit tested offline.
- Share pitch-sweep helper with kick/tom.
- No overlap with **MpcLaSynth** PCM — pure synthesis only.

## Line placement

Drum **synthesis** vs future sample chopper SKUs; pair with **MpcSh101** bass on productions.