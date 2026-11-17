# MpcCs80 — APC plan

**Next:** `/apc-design MpcCs80`

## Complexity

| Dimension | Score | Notes |
|-----------|-------|-------|
| Voices | 3 | 4 (dual layer = 2× DSP path per note) |
| Oscillators/voice | 4 | 2 layers × 2 VCO + sub |
| Filters | 3 | HPF + LPF × 2 layers |
| Modulation | 3 | PWM LFO, ring mod, poly AT |
| FX | 0 | v1 dry (ensemble v2) |
| Surfaces | 1 | headless + programs |
| Targets | 1 | x86_64 + aarch64 |
| **Total** | **15** | |

**Tier:** **L3** (heaviest in MPC classic poly row until Ob-Xa split oscillators)

## UI framework

`headless` — Melodee K1–K3 + program bank.

## Implementation strategy

1. **Prove one layer** in unit test / offline render (PWM + HPF/LPF + ADSR).
2. **Clone to Layer II** with independent detune/PWM defaults.
3. **Voice manager + poly AT** map from MIDI buffer.
4. **Factory programs** for Blade Runner brass (high res, slow filter env, ring subtle) and string pad (low PWM, layer II forward).
5. Cross-build `build-vst-aarch64-linux.sh` / Podman; cap voices if >20% CPU per instance.

## Risks

- Dual-layer × 4 voices ≈ 8 filter stacks — main CPU cost; HPF/LPF must use efficient `StateVariableTPT` or fixed 2-pole IIR.
- Poly aftertouch requires Melodee/host to send poly pressure; channel pressure fallback in v1.1.

## Out of scope v1

- Ribbon controller emulation, per-layer independent filters (use shared cutoff + slight layer detune), chorus ensemble, full 8-voice CS-80 parity.