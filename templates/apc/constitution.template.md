# {{PLUGIN_NAME}} — DPCS Phase 0 Constitution

**Status:** Phase 0 · **Next:** Phase 1 PRD (`.ideas/creative-brief.md`, `.ideas/parameter-spec.md`) 
**Class:** {{PLUGIN_CLASS}} · **Category:** `{{CATEGORY}}` · **Created:** {{DATE}} 
**SOP:** [SOP-VF-01 Intake & Dream](../../docs/disklordz/sops/SOP-VF-01-intake-dream.md)

## Purpose

<!-- One paragraph: what the product is, who it is for, and why it exists now (market SKU, dogfood, client). -->

## Scope (in)

- Formats: VST3 (+ Standalone via template)
- DSP modules (factory-lib ids from `disklordz/vst-factory/modules/dsp/*.json`):
- GUI modules (from `disklordz/vst-factory/modules/gui/controls.json`):
- Design-system theme: `{{THEME}}`
- Target OS primary:

## Exclusions (out)

- <!-- Explicit non-goals. Anything not listed in Scope is out by default. -->

## Stack

| Layer | Choice |
|-------|--------|
| Framework | JUCE via `vst/template` or `vst/template-effect` |
| Spec | `factory-spec/v2`, category `{{CATEGORY}}` |
| UI | Factory design system, theme `{{THEME}}` |
| Build | `tools/build-vst.sh {{PLUGIN_NAME}}` |
| Verify | `run_business.py --profile factory --plugin {{PLUGIN_NAME}}` |
| Methodology | [docs/DPCS.md](../../docs/DPCS.md) |
| Gates | `approve_architecture` before scaffold · `approve_gui` before editor sign-off · `approve_release` before ship |

## Non-negotiables

1. **Component-first** — reusable DSP and GUI live in `vst/factory-lib/`, never duplicated in `Source/`.
2. **DONE = exit 0** — every phase ends with a script that exits 0.
3. **Phase 1 zero code** — behaviour docs only until Phase 2 headers.
4. **One module per Phase 3 run** — explicit file allowlist in each handoff.
5. **Realtime** — no allocation, locks, I/O or logging in `processBlock`.
6. **Theme roles only** — UI cites design-system role names, never hex values.

## Acceptance (Phase 4 targets)

```bash
tools/build-vst.sh {{PLUGIN_NAME}}
python3 vst-testing-ops/run_business.py --profile factory --plugin {{PLUGIN_NAME}}
bash plugins/{{PLUGIN_NAME}}/scripts/verify-phase4.sh
```

## Approvals

Operator notifications: Slack `#disklordz-factory-dept` (see [VST_FACTORY_SLACK_SETUP.md](../../docs/VST_FACTORY_SLACK_SETUP.md)).
