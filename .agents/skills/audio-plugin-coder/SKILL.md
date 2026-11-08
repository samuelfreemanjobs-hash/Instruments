---
name: audio-plugin-coder
description: Master APC orchestrator for Instruments — routes /apc-* phases to workflows and skills. Based on Noizefield APC workflow.
---

# Audio Plugin Coder (APC orchestrator)

## Authority

- `AGENTS.md`
- `docs/apc-workflow.md`
- `docs/dsp-architecture-specification.md`

## Command routing

| User types | Load |
|------------|------|
| `/apc-setup` | `.agents/workflows/apc-setup.md` + `apc-setup/SKILL.md` |
| `/apc-dream` | `apc-dream.md` + `dream/SKILL.md` |
| `/apc-plan` | `apc-plan.md` + `plan/SKILL.md` |
| `/apc-design` | `apc-design.md` + `design/SKILL.md` |
| `/apc-impl` | `apc-impl.md` + `impl/SKILL.md` |
| `/apc-test` | `apc-test.md` + `test/SKILL.md` |
| `/apc-debug` | `apc-debug.md` + `debug/SKILL.md` |
| `/apc-ship` | `apc-ship.md` + `ship/SKILL.md` |
| `/apc-status` | `apc-status.md` |
| `/apc-resume` | `apc-resume.md` |
| `/apc-new` | `apc-new.md` |

Aliases: `/dream` = `/apc-dream`, etc.

## Always

1. Read `plugins/<Name>/status.json` before mutating plugin state.
2. One phase per user request unless `/apc-new`.
3. Point implementation at `vst/<Name>/`, never only `plugins/<Name>/`.

## Upstream

Inspired by [Noizefield/audio-plugin-coder](https://github.com/Noizefield/audio-plugin-coder) (MIT). This repo integrates APC with NTS-1 logue assets and local `vst/template`.
