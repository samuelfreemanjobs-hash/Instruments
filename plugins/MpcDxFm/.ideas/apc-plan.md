# MpcDxFm — APC plan

**Next:** `/apc-design MpcDxFm`

## Complexity (spec §8)

| Dimension | Score | Notes |
|-----------|-------|-------|
| Voices | 3 | 16× 4-op target on ARM |
| Oscillators/voice | 3 | 4–6 op FM stack |
| Filters | 0 | none v1 |
| Modulation | 2 | global ADSR + LFO; per-op EG deferred |
| FX | 0 | |
| Surfaces | 1 | headless + programs |
| Targets | 1 | x86_64 + aarch64 |
| **Total** | **10** | |

**Tier:** **L2**

**Risk flags:**

- `sin()` per sample × ops × voices — profile early; fallback to **wavetable osc** if needed.
- **6-op** doubles op count — gate behind `voice_mode` and lower max voices to 8.

**Melodee upside:** FM avoids ladder/ZDF cost → **higher poly** than VA line on same CPU.

## UI framework

`headless` — K1=brightness, K2=feedback, K3=algo (or program step).

## Strategy

- `Source/Dsp/` only; no JUCE in op headers (unit-test algorithms with offline renders).
- Golden: one WAV per program vs reference tone (loose spectral match, not ROM dump).
- Optional shared module `shared/dsp/fm_operator.h` if second FM SKU appears.

## Relation to VA line

Complement **MpcJuno106** pads with **MpcDxFm** EP/bell; mono bass still **MpcSh101** / **MpcMinimoog**.