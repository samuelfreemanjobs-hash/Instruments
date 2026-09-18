---
name: dsp-parameter-mapping
description: Maps UI/host normalized values to DSP units with curves and smoothing. Use when knobs sound stepped, filters zip, or porting 0-100 logue presets to VST 0-1 or mkII 0-1023.
---

# DSP Parameter Mapping subagent

## Read first

`docs/dsp-architecture-specification.md` — **§7 Parameter mapping**

## Mapping pipeline

Implement in this order:

1. **Raw** — APVTS / hardware integer
2. **Normalized** — 0..1 or enum index
3. **DSP** — Hz, seconds, linear gain
4. **Smoothed** — audio-thread state updated per block or per sample

## Standard curves

| Use case | Function |
|----------|----------|
| Cutoff Hz | Log sweep between `fMin`, `fMax` |
| Attack/release | `t = tMin * pow(tMax/tMin, norm)` or squared norm |
| Gain dB | `pow(10, norm * (dBMax-dBMin)/20)` |
| logue 0–100 | `norm = value * 0.01f` |
| mkII 10-bit | `param_10bit_to_f32(value)` |

## Smoothing

- One-pole: `state += (target - state) * coef`
- `coef` from `smoothingMs` and `sampleRate` at prepare time
- Store targets from control thread; update state only in `processBlock`

## Mod matrix (L2+)

- Sources evaluated in program chain
- Apply as offset to **smoothed** base value unless spec says otherwise

## File placement

- `Source/Dsp/ParameterMap.h` — inline converters
- `Source/Dsp/ParameterSmoother.h` — per-param state array

## Deliverables

- Comment each param in `createParameterLayout` with curve type
- Document preset value semantics (0–100 vs normalized)

## Checklist

- [ ] No divide-by-zero in log curves
- [ ] Enum params clamped to valid range
- [ ] logue `OSC_PARAM` and VST APVTS use same DSP function bodies where shared
