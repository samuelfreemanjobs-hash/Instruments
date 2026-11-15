# MpcJuno106 — APC plan

**Architecture:** [architecture.md](architecture.md) · **Next:** `/apc-design MpcJuno106`

## Complexity (spec §8)

| Dimension | Score | Notes |
|-----------|-------|-------|
| Voices | 2 | 6 (Melodee) / 8 desktop |
| Oscillators/voice | 1 | DCO saw+pulse+sub |
| Filters | 1 | single 24 dB LP |
| Modulation | 1 | 1 LFO → PWM optional |
| FX | 1 | stereo chorus post-sum |
| Surfaces | 1 | headless + factory programs |
| Targets | 1 | VST3 x86_64 + aarch64 |
| **Total** | **8** | |

**Tier:** **L1** (Standard)

**Risk flags:** fixed voice pool only; chorus delay max 20–30 ms; no oversampling v1.

## UI framework

| Field | Value |
|-------|-------|
| `ui_framework` | `headless` |
| Decision | Melodee host; optional `MPC_VST3_HEADLESS=ON` |
| Rationale | Juno identity is chorus + filter; UI is knob map |

## Implementation strategy

- Reuse patterns from `vst/MpcAnalogSynth` (Synthesiser voice, APVTS order).
- Extract DCO + LPF to `Source/Dsp/` before ship (L1 compliance).