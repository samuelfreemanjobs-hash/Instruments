---
name: drumsmith
description: Routes DRUMSMITH kit work across W1–W5 — SPEC, BUILD, VERIFY (CALIPER), PACKAGE, IMPROVE — and picks the right sibling skill. Use when the user says DRUMSMITH, drum kit agent, Tail Light Static, native_factory, or asks which drumsmith skill to run.
---

# DRUMSMITH (router)

## Read first

| Layer | Path |
|-------|------|
| Entrypoint | `claude/drumsmith/CLAUDE.md` |
| Policy | `SOUL.md`, `DUTIES.md`, `SYSTEM_PROMPT.md` |
| Agent map | `claude/drumsmith/agent.yaml` |
| CALIPER (Cursor) | `claude/drumsmith/docs/CALIPER_CURSOR.md` |
| Workbench loop | `claude/drumsmith/docs/WORKBENCH_LOOP.md` |
| Memory | `python3 scripts/agent_memory.py recall --agent drumsmith <kw>` |

## Skill map

| Need | Skill |
|------|--------|
| Market / ICP / competitors | `customer-and-competition-intel` |
| Sourced dig | `deep-research` |
| Research → INTAKE | `research-to-product` |
| Kit SKU / folders / devices | `product-design` |
| Trap/phonk loop blueprint | `trap-drum-loop-architect` |
| MIDI feel → groove card | `learn-midi-groove` |
| Voice / native / render DSP | `audio-dsp-engineering` |
| Recipe from reference WAV | `drum-sample-reverse-engineering` |
| Meters / qa-report | `audio-analysis` |
| Skeptical release audit | CALIPER (`docs/CALIPER_CURSOR.md`) |

Package-local Claude skills (still valid): `kit-product-spec`, `drum-voice-recipes`, `sample-delivery-qa` under `claude/drumsmith/.claude/skills/`.

## W1–W5 dispatch

1. **W1 SPEC** — `product-design` / `research-to-product`; no audio claims  
2. **W2 BUILD** — `audio-dsp-engineering` + `native_factory` / genre factory (`g++-12`)  
3. **W3 VERIFY** — `audio-analysis` + **CALIPER** on JSON reports  
4. **W4 PACKAGE** — only after technical PASS + required HITL; else stay `release_ready=false`  
5. **W5 IMPROVE** — human notes from workbench → one change → re-VERIFY  

## Hard rules

- Technical PASS ≠ commercial ready  
- No listening claims without human notes  
- Device profiles stay UNVERIFIED until tested  
- Recall/add `drumsmith` memory before/after non-trivial kit runs  

## Scripts

```bash
cd claude/drumsmith
python3 scripts/learn_midi_groove.py --help
python3 scripts/trap_loop_architect.py --help
python3 scripts/verify_kit.py --kit PATH --manifest PATH/manifest.json --json PATH/qa-report.json
```
