# DRUMSMITH — Drum Sample & Kit Product Designer + DSP Audio Engineer
> Four-layer architecture. Fail-closed. DONE = a command that exits 0.

## What it does
Designs drum kit products (concept, hit list, naming, export plan, QA gate) and the DSP behind the sounds (voice recipes, real-time-safe render core, measurement). Built for Disklordz kits, the Tail Light Static kit, and SEANCE BOX samples.

## Quickstart
```bash
python3 scripts/self_test.py            # must exit 0 before first use
python3 scripts/verify_kit.py kits/MyKit # delivery checks on a folder of WAVs
```
Copy this folder to a repo root so `CLAUDE.md` and `.claude/` auto-load in Claude Code.

**Workbench (UI + docked agent):** [DRUMSMITH Workbench](https://claude.ai/artifact/FwE8VaPi2a5vXRMPoX19zH) — in-browser verify matches `verify_kit.py`; see `project.md` § Workbench v1.

## Architecture Decision Record
```
GATE 1 CONTROL LEVEL       Moderate: Sam checkpoints spec, ship, publish   -> not fully autonomous
GATE 2 PROBLEM COMPLEXITY  Multi-domain (product + DSP), predictable flow  -> sequential workflows
GATE 3 RESOURCES           Limited budget, solo founder                    -> single agent
GATE 4 DOMAIN EXPERTISE    Two linked domains, shared vocabulary           -> single agent + skills

SELECTED PATTERN:  Single agent + 3 skills + 1 read-only evaluator subagent (CALIPER, T3 only)
RATIONALE:         Product and DSP decisions share context (a hit list is a promise the DSP must keep).
                   Splitting them would cost 10-15x tokens for little gain. QA stays separate so the
                   work does not grade itself.
UPGRADE TRIGGERS:  Parallel kit lines run by separate sessions; melodic Sample Factory scope added.
TOKEN COST EST:    [A] single agent baseline; CALIPER adds one extra read-only pass on T3 only.
```

## Layers
| Layer | File | Owns |
|-------|------|------|
| L1 | SOUL.md | identity, voice, values |
| L2 | DUTIES.md | scope, tiers, HITL gates, hard halts, known gaps |
| L3 | system-prompt.md | intake, 5 workflows, tool rules |
| L4 | core-config.xml | env gates, models, delivery profile, halts |

Priority: L1 overrides all; conflicts are resolved by hierarchy and logged, never silent.

## Contents
- `.claude/agents/caliper.md` — independent QA subagent
- `.claude/skills/drum-voice-recipes` — cowbell, 808, kick, snare/clap/hats, bus, aliasing, real-time rules
- `.claude/skills/kit-product-spec` — hit list, folder tree, naming, device export, provenance
- `.claude/skills/sample-delivery-qa` — verify_kit.py usage and what it cannot judge
- `scripts/dsp_math.py`, `verify_kit.py`, `self_test.py` — stdlib only

## Known gaps
See DUTIES.md. Short version: it cannot listen, phonk numbers are presets, device profiles are unverified, true peak is approximate.
