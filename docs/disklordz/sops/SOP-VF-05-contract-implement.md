# SOP-VF-05 — Contract & Implement (DPCS Phase 2–3)

## Purpose

Write code in the safest order: **headers that compile** (contract), then **one module per run** (implementation), program chain before surface chain.

## Preconditions

- `bash tools/apc-gate.sh <Name> impl` exits 0.
- Factory gate `approve_architecture` approved (scaffold is refused otherwise).

## Procedure — Phase 2 Contract

1. Scaffold: `./tools/new-vst.sh <Name> --type <effect|synth>` (or factory `POST /api/projects/{id}/scaffold`). Set unique `PLUGIN_CODE`, product name, `COPY_PLUGIN_AFTER_BUILD FALSE`.
2. New reusable blocks → headers in `vst/factory-lib/<area>/` from `templates/factory/factory-lib-block.template.h` ([SOP-VF-10](SOP-VF-10-reusable-module.md)). Product-only code → `vst/<Name>/Source/*.h`.
3. Declarations, doc comments citing architecture sections, `static_assert`s for realtime assumptions (e.g. lock-free atomics). No bodies.
4. Compile-check (cmake configure + build the headers into a stub TU). Commit: `"<Name>: DPCS Phase 2 contract"`.

## Procedure — Phase 3 Modules

1. One module per commit (and per agent run): the allowlist is the module's `.h/.cpp` and its tests.
2. Order: JUCE-free factory-lib blocks → `PluginProcessor` (program chain) → `params.json` sync → editor (surface chain, design-system components only).
3. Each module ships with its tests in the same commit; each test names the contract it protects.
4. Realtime rules in `processBlock`: no allocation, locks, I/O, logging; `ScopedNoDenormals`; handle `numSamples == 0`.
5. Editor: layout + attachments only; `factory_gui_use_theme(<Name> THEME <id>)`; no colours or fonts outside tokens.
6. Update `status.json`: `code_complete` when all modules are in.

## Definition of done

- Every module's tests pass; `tools/build-vst.sh <Name>` exits 0.
- No GUI header included from DSP code (`rg "juce_gui" vst/factory-lib/analysis vst/<Name>/Source/Dsp` is empty).

## Pitfalls

- Building the editor before the design-system components exist → hand-rolled colours. Use `GenericAudioProcessorEditor` until they land.
- Copy-after-build on Windows writes into the system VST3 folder (admin, live DAW plugins). Keep it off.

## Related

[SOP-VF-06 Verify](SOP-VF-06-verify-test.md) · [.agents/skills/impl/SKILL.md](../../../.agents/skills/impl/SKILL.md) · [dsp-architecture-specification.md](../../dsp-architecture-specification.md)
