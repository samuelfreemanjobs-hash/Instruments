# MeterTap — DPCS Phase 0 Constitution

**Status:** Phase 0 complete · **Phase 1:** [DPCS_PHASE1_PRD.md](DPCS_PHASE1_PRD.md) · **Next:** Phase 2 Contract (headers only)

## Purpose

Internal **factory dogfood** effect: stereo peak/RMS meter proving `vst/factory-lib` analysis block, factory QA profiles, DPCS discipline, and approval gates before market-research SKU #1.

## Scope (in)

- VST3 + optional standalone via APC scaffold (`plugins/MeterTap/`, `vst/MeterTap/`)
- **One** DSP module: `analysis.peak_rms` (factory-lib)
- **One** GUI module: `gui.meter`
- Params: input trim (`dl_mix_level` or taxonomy-aligned ids), meter ballistics defaults per spec
- Target OS primary: **Windows x64**; CI proof on **Linux compile** (tier A QA)

## Exclusions (out)

- No bespoke filter/osc/synth DSP invented in-plugin
- No CHORD ENGINE / DPF code paths
- No preset bank V1 (optional stub manifest only)
- No AAX, no iLok, no copy protection in dogfood tranche
- No cloud telemetry or phone-home

## Stack

| Layer | Choice |
|-------|--------|
| Framework | JUCE via `vst/template` |
| Spec | `factory-spec/v2`, category `fx.metering` |
| Build | `tools/build-vst.sh MeterTap` |
| Verify (Phase 4) | `run_business.py --profile factory`; `factory-audio` when WAV fixture exists |
| Methodology | [docs/DPCS.md](../../docs/DPCS.md) |
| Gates | `approve_architecture` before scaffold; `approve_release` before ship |

## Non-negotiables

1. **Component-first** — meter DSP lives in factory-lib, not duplicated in `Source/`.
2. **DONE = exit 0** — no subjective “looks fine.”
3. **Phase 1 zero code** — behaviour docs only until Phase 2 headers.
4. **One `.cpp` per Phase 3 run** — explicit allowlist in handoff.
5. **Realtime** — no alloc/I/O in `processBlock` (see `docs/dsp-architecture-specification.md`).

## Acceptance (Phase 4 targets)

```bash
tools/build-vst.sh MeterTap
python3 vst-testing-ops/run_business.py --profile factory --plugin MeterTap
```

Windows when online: pluginval + DAW smoke (not CI blocker).

## Factory intake prompt (reference)

From `disklordz/vst-factory/data/meter-tap-dogfood.json`:

> DiskLordz factory dogfood: stereo peak and RMS meter effect with dl_mix_level gain and gui.meter; uses factory-lib analysis.peak_rms only.

## Approvals

Operator notifications: Slack `#disklordz-factory-dept` — see [VST_FACTORY_SLACK_SETUP.md](../../docs/VST_FACTORY_SLACK_SETUP.md).
