---
name: logue-mkii-oscillator
description: Orchestrates logue SDK v2 mkII custom oscillator work — brief through ship for NTS-1 mkII. Routes /logue-mkii-* slash commands.
---

# logue SDK mkII custom oscillator agent (orchestrator)

## Authority

- **System prompt:** `.cursor/agents/logue-mkii-oscillator/SYSTEM.md`
- **Workflow guide:** `docs/logue-mkii-workflow.md`
- **Porting:** `docs/nts1-mkii-porting.md`
- **Skill:** `.cursor/skills/logue-hardware-port/SKILL.md`
- **Golden unit:** `tr808_kick_phonk` — `docs/logue-mkii-golden-unit.md`

## Command router

| User invokes | Run workflow | Then skill |
|--------------|--------------|------------|
| `/logue-mkii-setup` | `logue-mkii-setup.md` | `logue-mkii-setup` |
| `/logue-mkii-brief [slug]` | `logue-mkii-brief.md` | `logue-mkii-brief` |
| `/logue-mkii-design [slug]` | `logue-mkii-design.md` | `logue-mkii-design` |
| `/logue-mkii-scaffold [slug]` | `logue-mkii-scaffold.md` | `logue-mkii-scaffold` |
| `/logue-mkii-impl [slug]` | `logue-mkii-impl.md` | `logue-mkii-impl` |
| `/logue-mkii-eval [slug]` | `logue-mkii-eval.md` | `logue-mkii-eval` |
| `/logue-mkii-ship [slug]` | `logue-mkii-ship.md` | `logue-mkii-ship` |
| `/logue-mkii-debug [slug]` | `logue-mkii-debug.md` | `logue-mkii-debug` |
| `/logue-mkii-new [slug]` | `logue-mkii-new.md` | sequential phases |
| `/logue-mkii-status [slug]` | `logue-mkii-status.md` | read status only |
| `/logue-mkii-resume [slug]` | `logue-mkii-resume.md` | continue from status |

## PM / backlog (Korg Logue Agent)

- **Backlog:** `docs/logue-oscillator-ideas-backlog.md`
- **Weekly intake:** `docs/logue-agent-pm-automation.md` · `tools/logue/weekly-ideas-intake.sh --print-agent-prompt`
- Do not re-propose slugs already `planned` or `shipped` in the backlog.

## Slug resolution

- **slug** = mkII folder name under `src/mkii/oscillators/<slug>/`
- If user gives a display name only, derive `snake_case` slug or ask once.
- State file: `src/mkii/oscillators/<slug>/.agent/status.json`

## Phase order (mandatory for new units)

`setup` (once) → `brief` → `design` → `scaffold` → `impl` → `eval` → `ship`

Do not skip **design** before **impl**. Do not claim **ship** without **eval** pass.

## Non-goals

- No `osc_process` / `user_osc.c` fiction API
- No quarantined Claude bundle (`docs/archive/quarantined-claude-osc-bundle/`)
- No JUCE inside `src/mkii/oscillators/`
