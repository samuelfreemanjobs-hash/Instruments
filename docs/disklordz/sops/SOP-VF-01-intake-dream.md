# SOP-VF-01 — Intake & Dream (DPCS Phase 0–1)

## Purpose

Turn an idea into a scoped constitution, a behaviour-only PRD and a parameter contract, with **zero code**.

## Inputs

- Idea or factory intake prompt (`disklordz/vst-factory/data/*.json` or the factory UI)
- Optional reference audio in `plugins/<Name>/references/`

## Procedure

1. **Workspace:** `bin/apc init <Name>` (PascalCase name, unique in `plugins/`).
2. **Templates:** `bin/apc template <Name> dream --class <effect|synth|midi|sampler> --category <taxonomy id> --theme disklordz`. Category ids: `disklordz/vst-factory/modules/product-taxonomy.json`.
3. **Constitution (DPCS 0):** fill purpose, scope in/out, stack, non-negotiables. Scope lists factory-lib DSP and GUI module ids; anything not listed is out.
4. **Creative brief (DPCS 1):** behaviour only. For every control, describe the full range and what must *not* happen. Fill the **silence and extremes** table (zero input, full scale, NaN/Inf, sample rates, block sizes 0 and 1), bypass and latency. Acceptance checks are numbered and have tolerances.
5. **Parameter spec:** one row per parameter: id (prefixed snake_case, never renamed after release), range, default, unit, automatable, smoothing. Map every parameter to a GUI binding or mark it "no widget".
6. **Self-review for contradictions:** units (peak vs RMS references), ranges that break stated limits, bypass semantics. Record anything unclear as an open question for the operator rather than guessing.
7. **Status:** set `dpcs_phase` 1, `validation.dpcs_phase0_constitution_exists`, `creative_brief_exists`, `parameter_spec_exists` true; `next_command` `/apc-plan <Name>`.
8. **Verify:** `bin/apc template <Name> verify` (creates `scripts/verify-phase1.sh`), then `bash plugins/<Name>/scripts/verify-phase1.sh`.

## Definition of done

- `bash plugins/<Name>/scripts/verify-phase1.sh` exits 0 (fails while template comments remain in the brief).
- `bash tools/apc-gate.sh <Name> plan` exits 0.
- No files under `vst/<Name>/`.

## Pitfalls (seen in practice)

- Parameter spec placed outside `.ideas/` → `apc-gate.sh plan` fails. Templates put it in the right place.
- Brief contradicting itself (e.g. "−12 dBFS sine reads −12 dB RMS") → caught in Plan; cheaper to catch here.
- Constitution and parameter spec naming the same control differently → parameter spec wins; fix the constitution.

## Templates

`constitution.template.md` · `creative-brief.template.md` · `parameter-spec.template.md` · `verify-phase1.template.sh`

## Related

[SOP-VF-00](SOP-VF-00-vst-factory-master.md) · [SOP-VF-04 Plan](SOP-VF-04-plan.md) · [.agents/skills/dream/SKILL.md](../../../.agents/skills/dream/SKILL.md)
