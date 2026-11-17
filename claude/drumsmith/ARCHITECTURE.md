# DRUMSMITH architecture

## Purpose

Verifiable drum kit production for Disklordz supply SKUs: spec → DSP recipes → render → `verify_kit.py` → CALIPER on T3 ship claims.

## Build & run

| Command | Output |
|---------|--------|
| `python3 scripts/self_test.py` | 19 unittest cases + verify demos, exit 0 |
| `python3 scripts/verify_kit.py <kit_dir>` | Per-file + pack delivery checks |

Run from `claude/drumsmith/` (or repo path above).

## Data flow

```text
INTAKE.md → W1 kit-spec.md → W2 voice tables → render WAVs → verify_kit.py → CALIPER → device export (gated)
```

## Layer files (canonical tarball layout)

| Layer | File |
|-------|------|
| L1 | `SOUL.md` |
| L2 | `DUTIES.md` |
| L3 | `system-prompt.md` |
| L4 | `core-config.xml` |

## Key modules

| Path | Responsibility |
|------|----------------|
| `scripts/dsp_math.py` | MIDI, loop length, glide, fades, ZC pitch, approx true-peak |
| `scripts/verify_kit.py` | 24-bit delivery profile + pack uniformity |
| `scripts/self_test.py` | Hand-derived contract tests |
| `.claude/skills/` | W1–W5 procedures |
| `.claude/agents/caliper.md` | Read-only QA subagent |
| `kits/<slug>/` | Spec + intake answers |
| `logs/` | JSONL audit (runtime) |

## Extension points

- Enable CALIPER gate in `core-config.xml` when Sam sets `evaluator_pass_enabled`.
- Fill `docs/provenance/<kit>.md` on first render batch.
- Device profiles: confirm then flip UNVERIFIED in spec.

## Workbench

| Path | Role |
|------|------|
| `workbench/DRUMSMITH_Workbench.html` | Single-file UI (verify + calculators + spec); agent when hosted |
| `workbench/README.md` | Local serve instructions |

## Related docs

- `project.md` — Rule #1 status log
- Root `ARCHITECTURE.md` — monorepo index