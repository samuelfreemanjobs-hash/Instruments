# Night Circuit — verification roadmap

**Product frame:** [ProphetRev2Trap/docs/MILESTONE_1.md](../ProphetRev2Trap/docs/MILESTONE_1.md) — playable instrument, Rev2-*inspired* workflow, synthetic trap identity (not Rev2 emulation).

**Policy:** Verification before features. Gates must pass before the next phase starts. **Milestone 1 complete** = Phase 1 + Phase 2 below (+ featured foundations signed by sound-design).

## Phase 1 — Working baseline (current focus)

- [x] Pinned JUCE **8.0.15** + documented Linux/g++-12 baseline → [ProphetRev2Trap/docs/BUILD_BASELINE.md](../ProphetRev2Trap/docs/BUILD_BASELINE.md)
- [x] Automated preset count, category quotas, unique names, parameter ranges → `NightCircuitTests` + `verify_preset_bank.py`
- [x] Processor smoke: MIDI note-on produces audio; state blob round-trip → `ProcessorSmokeTests.cpp`
- [x] pluginval VST3 smoke (Linux CI)
- [x] Editor minimum width **960** px (top-row preset controls)
- [ ] **Manual:** Standalone or DAW load + MIDI listen (owner machine or QA agent with audio)

**Gate:** Reproducible build + playable plugin. **No feature work until pass.**

## Phase 2 — Dependable instrument

- Mono priority, sustain pedal, voice steal, pitch bend, mono/poly switching
- Aliasing, resonance stability, denormals, CPU, levels @ 44.1 / 48 / 96 kHz
- Harden user-preset import (size limits, schema, param validation)
- Automated state-recall + parameter automation tests

**Gate:** No stuck notes, unsafe audio-thread work, broken recall, or critical DSP defects.

## Phase 3 — Preset quality (quantity → quality)

- **QA agent:** silence, duplicates, loudness outliers, render failures
- **Sound-design lead:** musical fit, retune/replace weak variations → [PRESET_SOUND_DESIGN.md](../ProphetRev2Trap/docs/PRESET_SOUND_DESIGN.md)
- Ship **50–100 featured** presets; full **1,028** remains browsable

## Phase 4 — Production workflow (after Phase 2)

One item per test pass: search/favorites/featured filters, mod wheel / velocity→filter, chorus + tempo delay, GUI a11y and units.

## Phase 5 — Distribution

Supported OS/DAW matrix (**FL Studio**, **MPC Software** primary), installer layout, versioning, licensing, crash policy, signing/notarization, docs, release checklist. See [ProphetRev2Trap/docs/HOST_COMPATIBILITY.md](../ProphetRev2Trap/docs/HOST_COMPATIBILITY.md).

## Roles

| Role | Owns |
|------|------|
| **VST Plugin Factory** | Implementation fixes |
| **Night Circuit QA** | Independent proof, Gates 1–2 automation |
| **Preset sound-design subagent** | Featured set, preset GUI UX, category listening — [NIGHT_CIRCUIT_SOUND_DESIGN_SUBAGENT.md](NIGHT_CIRCUIT_SOUND_DESIGN_SUBAGENT.md) |
