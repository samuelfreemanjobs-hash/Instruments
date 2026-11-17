# DRUMSMITH — project log (Rule #1)

**Updated:** 2026-11-17  
**Status:** Agent package in-repo · First live kit spec started (Tail Light Static)

## Scope

Four-layer agent (FORGE template): drum **kit product design** + **DSP engineering**. Orchestrator **DRUMSMITH**; read-only QA subagent **CALIPER**. Python stdlib verify scripts; hands instrument/plugins to **FORGE**.

## Repo layout

| Path | Role |
|------|------|
| `claude/drumsmith/CLAUDE.md` | Session charter when cwd is package |
| `claude/drumsmith/INTAKE.md` | 13-question kit intake (filled per kit) |
| `claude/drumsmith/layers/` | L1–L4 agent template |
| `claude/drumsmith/.claude/` | Agents + skills for Claude Code |
| `claude/drumsmith/scripts/` | `dsp_math.py`, `verify_kit.py`, `self_test.py` |
| `claude/drumsmith/kits/<slug>/` | Per-kit spec + provenance |

## Verification

```bash
python3 claude/drumsmith/scripts/self_test.py   # 19 tests, exit 0
python3 claude/drumsmith/scripts/verify_kit.py path/to/renders --bpm 74
```

## Known gaps

- No listening; CALIPER is measurement-only.
- Phonk numeric presets tagged `[A]` (tunable).
- Device export profiles **UNVERIFIED** until `disklordz_slicer.py` or manual confirm.
- True-peak in checker is sample-peak approximation.

## Next

- [x] Land package in Instruments monorepo (`cursor/drumsmith-tail-light-static-58c6`)
- [x] Fill INTAKE + W1 spec for Tail Light Static
- [ ] Render WAVs + `verify_kit.py` on real kit folder
- [ ] Confirm device profiles; enable CALIPER gate in `core-config.xml`