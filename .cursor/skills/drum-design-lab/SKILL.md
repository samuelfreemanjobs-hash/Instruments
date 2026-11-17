---
name: drum-design-lab
description: >-
  Orchestrates DiskLordz Drum Design Lab — portable C++20 drum synthesis,
  MultiGenre kit styles, Intelligent Lab, overnight agent, and optional JUCE
  Standalone/VST3. Use when the user mentions Drum Design Lab, DDL, MultiGenre,
  genre drum engines (Tay Keith, Toomp, Purps, Shawty, Hitmaker), procedural
  kits, or building the drum standalone.
---

# Drum Design Lab (orchestrator)

## Authority

- Product: `vst/DrumDesignLab/ARCHITECTURE.md`
- APC workspace: `plugins/DrumDesignLab/`
- Windows worker: `.cursor/rules/windows-worker.mdc` — worktree, one heavy job, `COPY_PLUGIN_AFTER_BUILD=OFF`
- Sub-skills: `genre-drum-engine`, `intelligent-drum-lab`, `overnight-kit-agent`, `ddl-standalone-build`

## Non-negotiables

1. **Zero third-party sample libraries** for core kits — procedural DSP only.
2. **No heap / file I/O / mutexes** on the audio thread (`renderTick` / `processBlock`).
3. Genre engines are **header-only portable C++20** (no JUCE in `Source/DSP/`).
4. Do **not** merge via PowerShell into fictional `DrumEngine.h` / `prd.json` — wire `KitStyle` in `MultiGenreDrumEngine.h`.
5. Never claim Standalone/VST3 built without an artifact path.

## Route the request

| User ask | Skill / action |
|----------|----------------|
| New genre / producer mode | `genre-drum-engine` |
| Prompt → batch kicks/808s | `intelligent-drum-lab` |
| Overnight evolution loop | `overnight-kit-agent` |
| Standalone or VST3 app | `ddl-standalone-build` |
| Core test / WAV export only | Build `build-core`, run `ddl_m1_core_test`, `DiskLordzBatchRenderer` |

## Layout

```text
vst/DrumDesignLab/
  Source/DSP/          # genre engines + MultiGenre facade
  Source/Lab/          # IntelligentDrumLab
  Source/Plugin/       # JUCE processor/editor
  Tests/m1_core_test.cpp
  Tools/render_kit_batch.cpp
overnight-agent/       # numpy overnight harness (separate)
plugins/DrumDesignLab/ # APC dream/plan/design
```

## Definition of done (engine change)

- [ ] Engine compiles in `build-core` Release
- [ ] `ctest` / `ddl_m1_core_test` PASS
- [ ] `KitStyle` + name + prepare/reset/MIDI/render wired
- [ ] `ARCHITECTURE.md` module row updated
- [ ] No secrets; no Program Files VST3 copy
