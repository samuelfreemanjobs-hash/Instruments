# MpcMinimoog — APC plan

**Architecture:** [architecture.md](architecture.md) · **Next:** `/apc-design MpcMinimoog`

## Complexity (spec §8)

| Dimension | Score | Notes |
|-----------|-------|-------|
| Voices | 1 | strictly monophonic |
| Oscillators/voice | 2 | 3 osc + noise (capped at score 2) |
| Filters | 3 | ladder + saturation/drive |
| Modulation | 1 | glide + slow drift |
| FX | 0 | none v1 |
| Surfaces | 1 | headless + programs |
| Targets | 1 | x86_64 + aarch64 |
| **Total** | **9** | |

**Tier:** **L2** (filter nonlinearity + dual envelope; still single voice)

**Risk flags:** ladder + drive at 44.1 kHz — profile on MPC Sample; cap `drive` default for Melodee Init Bass.

**Melodee upside:** ~1× voice cost vs 6× poly — default **heavy emphasis** patches are viable.

## UI framework

| Field | Value |
|-------|-------|
| `ui_framework` | `headless` |
| Decision | K1–K3 = cutoff, emphasis, glide |

## Implementation strategy

- New `vst/MpcMinimoog/` after Design (do not overload `MpcAnalogSynth` poly voice).
- Reuse glide + smoothing from `shared/dsp/parameter_smoothing.h` if present.
- Unit test: `LadderFilter` stability at emphasis 0.95, block 128.