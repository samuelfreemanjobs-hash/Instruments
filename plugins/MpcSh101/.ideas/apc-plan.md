# MpcSh101 — APC plan

**Next:** `/apc-design MpcSh101`

## Complexity (spec §8)

| Dimension | Score | Notes |
|-----------|-------|-------|
| Voices | 1 | mono |
| Oscillators/voice | 1 | VCO + sub |
| Filters | 1 | single VCF |
| Modulation | 1 | glide + optional PWM LFO |
| FX | 0 | |
| Surfaces | 1 | headless + programs |
| Targets | 1 | |
| **Total** | **6** | |

**Tier:** **L1**

**Risk flags:** none beyond standard RT; simplest Melodee bass SKU after MeterTap-scale FX.

**Pairing:** ship on SD with **MpcMinimoog** (cream ladder) + **MpcSh101** (punch/acid).

## UI framework

`headless` — K1=cutoff, K2=res, K3=sub.

## Strategy

Impl after Design; share `Glide.h` with MpcMinimoog if extracted in that Design phase.