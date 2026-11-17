# DRUMSMITH architecture

## Purpose

Verifiable drum kit production for Disklordz supply SKUs: spec → DSP recipes → render → `verify_kit.py` → optional device export.

## Build & run

| Command | Output |
|---------|--------|
| `python3 scripts/self_test.py` | 19 contract tests, exit 0 |
| `python3 scripts/verify_kit.py DIR --bpm 74` | Kit QA report |

## Data flow

```text
INTAKE.md → W1 kit-spec.md → W2 voice tables → render WAVs → verify_kit.py → CALIPER → device export (gated)
```

## Threading / realtime

Package scripts are offline QA only. Voice RT rules in specs are constraints for FORGE/JUCE renderers.

## Key modules

| Path | Responsibility |
|------|----------------|
| `scripts/dsp_math.py` | Bar/beat sample counts, dBFS helpers |
| `scripts/verify_kit.py` | 24-bit mono kit checks |
| `layers/` | Agent template L1–L4 |
| `.claude/skills/` | W1–W5 procedures |
| `kits/<slug>/` | Per-kit specs |

## Extension points

- Set `<evaluatorPassEnabled>true</evaluatorPassEnabled>` when CALIPER gate is mandatory.
- Add device profile verification doc beside each `UNVERIFIED` row.
- `docs/provenance/<kit>.md` on first render batch.

## Related docs

- `project.md` — Rule #1 log
- Repo `plugins/MemphisPhonkFactory/` — phonk drum reference
- `vst-testing-ops/` — plugin golden QA (separate lane)