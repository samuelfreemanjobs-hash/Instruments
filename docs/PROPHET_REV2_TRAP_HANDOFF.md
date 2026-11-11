# Agent handoff — Prophet Rev2 Trap (Cursor ↔ Claude Code)

**Updated:** 2026-11-07 · **Do not edit the same files in parallel without reading this.**

## Canonical git state (Cursor Cloud)

| Item | Value |
|------|--------|
| Branch | `cursor/prophet-rev2-trap-223b` |
| PR | [#109](https://github.com/samuelfreemanjobs-hash/Instruments/pull/109) → `main` |
| Base | User pasted `Rev2Voice` + `ProphetRev2Processor` DSP; Cursor wrapped it in JUCE + CMake |

**Claude Code:** `git fetch origin cursor/prophet-rev2-trap-223b && git checkout cursor/prophet-rev2-trap-223b` (or rebase your branch onto this SHA after pull).

## Shipped on `cursor/prophet-rev2-trap-223b`

- `ProphetRev2Trap/Source/ProphetRev2Engine.h` — 8 voices, TPT 2/4-pole filter, slop, bass comp, `fastTanh` sat, 3 internal `loadPreset()` programs
- `PluginProcessor.*` — APVTS, MIDI note on/off → `ProphetRev2SynthEngine`, `pushParametersToEngine()`
- `PluginEditor.*` — program, cutoff, resonance, slop, glide (ms), saturation combo
- `FactoryPresets.h` — Phonk Brass / Rev2 Trap Pluck / Glide Pad (APVTS-aligned)
- `ProphetRev2Trap/CMakeLists.txt` — VST3 + Standalone; root `CMakeLists.txt` `add_subdirectory(ProphetRev2Trap)`
- `ProphetRev2Trap/ARCHITECTURE.md` + root `ARCHITECTURE.md` index row

## Build (verified on Linux CI VM)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build --target ProphetRev2Trap_VST3
```

Artifact: `build/ProphetRev2Trap/ProphetRev2Trap_artefacts/Release/VST3/Prophet Rev2 Trap.vst3`

**VM note:** parallel `cmake --build -j8` can OOM if pytest/other jobs hold RAM; prefer `-j2`–`-j4`.

## Parameter contract (APVTS → engine)

| APVTS ID | Engine / behavior |
|----------|-------------------|
| `glide_ms` | `engine.setGlide(ms * 0.001f)` — **seconds** inside engine (`param_glide`) |
| `cutoff_hz`, `resonance`, `bass_comp`, `slop`, `crunch` | Smoothed in engine per block |
| `saturation_on` | `AudioParameterChoice` Off/On → `setSaturation(int)` |
| `filter_mode` | 0 = 4-pole, 1 = 2-pole |
| `program` | `FactoryPresets::applyPatch` |

Envelope ADSR params are still driven from factory presets / engine defaults unless Claude adds APVTS IDs.

## Out of scope on PR #109 (good Claude Code tasks)

1. **16-step sequencer** from original product spec — not wired
2. **Full APVTS** for per-ADSR (amp/filter) sliders if you want host automation on envelopes
3. **CI** — add `ProphetRev2Trap_VST3` to `.github/workflows/build.yml` or `scripts/vst/autopilot-compile.sh`
4. **APC lifecycle** — `plugins/ProphetRev2Trap/` dream/plan JSON if you follow `docs/vst-lifecycle.md`
5. **`pluginval` / golden render** — reuse `vst-testing-ops/run_business.py` pattern from main JUCE plugin

## Conflict avoidance

| Owner suggestion | Paths |
|------------------|--------|
| Cursor already touched | `ProphetRev2Trap/**`, root `CMakeLists.txt` (subdirectory line), `ARCHITECTURE.md` (index row) |
| Safe parallel lanes | Sequencer in new `Source/StepSequencer.*`, tests under `ProphetRev2Trap/Tests/`, CI yaml only |
| Other active branches | `cursor/ai-coding-agent-stack-223b` (Grok PWA — **no** Prophet overlap), `cursor/vst-engineer-cursor-setup-223b` (rules/docs) |

## Related repo context

- VST engineer rules (if on another branch): `.cursor/rules/vst-engineer.mdc`, `docs/CURSOR_VST_ENGINEER.md`
- Monorepo agent index: `AGENTS.md` → read `ARCHITECTURE.md` first

## Paste packet for Claude Code (if not using git)

```
Work: Prophet Rev2 Trap JUCE VST3
Branch: cursor/prophet-rev2-trap-223b PR #109
DSP core: ProphetRev2Trap/Source/ProphetRev2Engine.h (ProphetRev2SynthEngine, 8× Rev2Voice)
JUCE shell: PluginProcessor.cpp / PluginEditor.cpp
Presets: FactoryPresets.h (3 programs); glide is glide_ms in APVTS, seconds in engine
Build target: ProphetRev2Trap_VST3
Before push: cmake build above; avoid duplicating DspCore.h — engine is single header
Handoff doc: docs/PROPHET_REV2_TRAP_HANDOFF.md
```
