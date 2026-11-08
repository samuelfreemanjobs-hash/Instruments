---
name: logue-hardware-port
description: Ports DSP to logue SDK v1.1 oscillators and mkII osc.h from VST or shared/dsp. Use for NTS-1, XD, Prologue, mkII — not for new VST-only plugins.
---

# logue Hardware Port subagent

## Read first

- `docs/nts1-mkii-porting.md`
- `docs/dsp-architecture-specification.md` — parameter mapping §7
- `.cursor/rules/01-sdk-api.mdc`, `03-build-targets.mdc`

## Targets

| Platform | Path | API |
|----------|------|-----|
| mkI / XD / Prologue | `src/oscillators/<unit>/` | `OSC_*`, q31 out |
| mkII | `src/mkii/oscillators/<slug>/` | `Osc::process`, float |

## Port workflow

1. Extract float DSP from VST `Source/Dsp/` → `shared/dsp/` if not already shared.
2. **v1.1:** Implement `OSC_CYCLE`; convert out with `f32_to_q31`; pitch via `osc_w0`.
3. **mkII:** Scaffold `python3 tools/mkii/scaffold-mkii.py oscillators/<unit>`; fill `osc.h` `process()`.
4. Align `manifest.json` params with `params.json` logical IDs.
5. Build: `./tools/build.sh` or `./tools/build-mkii.sh`.

## Parameter mapping

- VST 0..1 → logue preset 0..100 → mkII 0..1023 per spec §7.2.

## Do not

- Include `userosc.h` in VST builds
- Include JUCE in `src/oscillators/`

## Deliverables

- Working `manifest.json` `num_param` ≤ 10 (mkII)
- PORTING.md updated on mkII scaffold
- Note CPU tier (Cortex-M4 vs M7)
