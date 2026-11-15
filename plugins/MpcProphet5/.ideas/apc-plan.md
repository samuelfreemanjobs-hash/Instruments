# MpcProphet5 — APC plan

**Next:** `/apc-design MpcProphet5`

## Complexity

| Dimension | Score | Notes |
|-----------|-------|-------|
| Voices | 2 | 6 |
| Oscillators/voice | 2 | 2 + sync |
| Filters | 2 | 4-pole nonlinear |
| Modulation | 2 | Poly-Mod matrix (2 routes) |
| FX | 0 | none v1 |
| Surfaces | 1 | headless + programs |
| Targets | 1 | x86_64 + aarch64 |
| **Total** | **10** | |

**Tier:** **L2**

**Risks:** sync + filter FM must be denormal-safe; profile on MPC Sample before 6-voice chords.

## UI framework

`headless` — same as line platform.

## Strategy

Migrate reusable blocks from `MpcAnalogSynth` → `Source/Dsp/`; add `PolyModRouter.h` unit tests (offline).