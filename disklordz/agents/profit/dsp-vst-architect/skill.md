---
name: disklordz-dsp-vst-architect
description: Elite JUCE/C++ DSP architect — desktop standalone, VST3/CLAP factory, CMake, realtime-safe audio, golden + pluginval gates.
when_to_use: User asks to build or fix audio plugins, JUCE processors, DSP algorithms, VST3 factory, desktop synth/sampler apps, CMake plugin targets, APVTS/state, SIMD voice rendering, or SP1200/Wave9090/JD Upgraded code.
disable-model-invocation: false
context: fork
paths:
  - "Source/**"
  - "Wave9090/**"
  - "SP1200/**"
  - "CMakeLists.txt"
  - "tools/**"
  - "tests/golden/**"
  - "vst-testing-ops/**"
  - "docs/ARCHITECTURE.md"
  - "disklordz/integrations/agents/dsp_vst_architect.py"
  - "disklordz/agents/profit/dsp-vst-architect/**"
  - ".github/workflows/build.yml"
---

# Skill — DSP & VST Factory Architect

## When to use

- **Greenfield** JUCE plugin or standalone desktop app (synth, sampler, effect)
- **DSP work**: filters, oscillators, envelopes, oversampling, saturation, spatial FX
- **Architecture**: APVTS, preset/state, MIDI/SysEx, multi-bus layouts
- **Factory**: new CMake targets, CLAP alongside VST3, CI integration
- **Quality**: pluginval failures, golden WAV drift, determinism / SpectralDiff
- **Performance**: SIMD voice paths, control-rate decimation, scratch buffers

## When *not* to use (route elsewhere)

| Topic | Agent |
|-------|--------|
| Golden-only regression triage without code changes | `golden-wav-qa` |
| Hardware preset SysEx / SynthForge | `hardware-preset-designer` |
| Disklordz SaaS checkout / Supabase | `conversion-qa`, `ops-schema` |
| Kit generation / WAV factory SaaS | `product-factory`, `async-generation` |

## Prerequisites

1. Read [`agent.md`](agent.md) and product `ARCHITECTURE.md` for the tree you touch.
2. Run fast sanity:

```bash
python3 disklordz/integrations/agents/dsp_vst_architect.py --self-test
python3 disklordz/integrations/agents/dsp_vst_architect.py --playbook
```

## Implementation checklist (elite bar)

- [ ] Audio path audited for allocations, locks, and blocking calls
- [ ] Parameters smoothed where audibility requires; denormals handled on DSP paths
- [ ] APVTS / state migration documented if layout changed
- [ ] `ARCHITECTURE.md` updated for new modules or extension row
- [ ] Verification `[executed]`: `run_business.py` (ci or dsp-only) or targeted `ctest`
- [ ] If golden output intentionally changed: `tests/golden/refresh_golden.sh` + manifest

## Success criteria

- [ ] CI-equivalent command output attached (pass or actionable fail tail)
- [ ] Code citations point to processor/DSP files, not vague “updated DSP”
- [ ] No secrets, ROM dumps, or licensed wave content committed

## Anti-patterns

- Fixing golden tests by widening tolerances without acoustic justification
- `new`/`malloc` per sample or per `processBlock` on the audio thread
- Skipping pluginval because “it works in standalone”
- Editing `disklordz/website/` when the task is plugin-only
- Large refactors outside the user’s DSP ask (R4 minimal fix)

## Invoke in Cursor

- Skill: **`disklordz-dsp-vst-architect`**
- Agent entry: **`disklordz/agents/profit/dsp-vst-architect/agent.md`**
