# SOP-VF-04 — Plan (architecture, complexity, UI framework)

## Purpose

Decide **how** the product works before any code: program chain, surface chain, global layer, reuse, verification plan and open issues.

## Inputs

Phase 1 docs (constitution, brief, parameter spec), [dsp-architecture-specification.md](../../dsp-architecture-specification.md), skill `.cursor/skills/dsp-complexity-assessment/SKILL.md`.

## Procedure

1. **Gate:** `bash tools/apc-gate.sh <Name> plan` must exit 0.
2. **Templates:** `bin/apc template <Name> plan`.
3. **Reuse search:** list candidate blocks in `shared/dsp/`, `vst/factory-lib/`, `disklordz/vst-factory/modules/dsp/*.json`. Note which exist as files vs catalog entries only (catalog entries without files become Phase 2 contracts).
4. **Program chain:** ordered per-block steps (denormals, sanitize, smoothing, core, output), with a reason for each decision (e.g. smoothing domain, fixed storage sizes).
5. **Realtime contract:** what the audio thread may and must not do; how data reaches the UI (atomics, lock-free hand-off).
6. **Surface + global:** APVTS ids from the parameter spec, timers, state, bus layouts, `prepareToPlay` work, plugin codes (check uniqueness: `rg "PLUGIN_CODE" -g CMakeLists.txt`).
7. **Verification plan:** numbered checks (V1…) with exact pass criteria and tolerances. Include build, factory QA and pluginval rows.
8. **Open issues:** every contradiction or gap in Phase 1 docs, with a chosen default. Ask the operator only for value-critical forks; record their answer in the brief.
9. **Complexity:** score the seven dimensions, tier L0–L3, risk flags. If the constitution forces more structure than the tier suggests, write an explicit `OVERRIDE`.
10. **UI framework:** `juce-native` unless there is a written reason for webview. Record theme.
11. **Status:** `architecture_defined`, `ui_framework_selected` true; `complexity_*`; `framework_selection`; `open_issues`; `next_command` `/apc-design <Name>`.

## Definition of done

- `bash tools/apc-gate.sh <Name> design` exits 0.
- `status.json` parses and the Phase 1 verify script still passes.

## Related

[SOP-VF-00](SOP-VF-00-vst-factory-master.md) · example: `plugins/MeterTap/.ideas/architecture.md`
