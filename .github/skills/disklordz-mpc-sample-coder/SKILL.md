---
name: disklordz-mpc-sample-coder
description: Akai MPC Sample (AC50) engineering — orchestrates .xpj, MPCVObject .xpm, Rust mods, virtual lab, SD autorun. Cursor lane skills under .cursor/skills/mpc-sample-*.
when_to_use: Profit lever — Faster AC50 kits and mods. Invoke for MPC Sample Coder fleet tasks.
disable-model-invocation: false
context: fork
paths:
  - "docs/MPC_SAMPLE*.md"
  - ".cursor/skills/mpc-sample-*/**"
  - "disklordz/trap-lord-synth/**"
  - "disklordz/agents/profit/mpc-sample-coder/**"
  - "disklordz/integrations/agents/mpc_sample_coder.py"
  - "disklordz/**"
  - ".github/workflows/**"
---

# Skill — MPC Sample Coder

## When to use

Akai MPC Sample (AC50) engineering: `.xpj` / `.xpm`, MPCTK, mpc-sample-sdk, Melodee, virtual-mpc-sample, Trap Lord keygroup export alignment.

## Cursor sub-skills (load with orchestrator)

| Skill id | Path |
|----------|------|
| `mpc-sample-coder` | `.cursor/skills/mpc-sample-coder/SKILL.md` |
| `mpc-sample-xpj` | `.cursor/skills/mpc-sample-xpj/SKILL.md` |
| `mpc-sample-xpm-keygroup` | `.cursor/skills/mpc-sample-xpm-keygroup/SKILL.md` |
| `mpc-sample-rust-mod` | `.cursor/skills/mpc-sample-rust-mod/SKILL.md` |
| `mpc-sample-virtual-lab` | `.cursor/skills/mpc-sample-virtual-lab/SKILL.md` |
| `mpc-sample-sd-autorun` | `.cursor/skills/mpc-sample-sd-autorun/SKILL.md` |

Agents mirror: `.agents/skills/mpc-sample-coder/SKILL.md`

## Prerequisites

- Read `docs/MPC_SAMPLE_ECOSYSTEM.md`, `docs/MPC_SAMPLE_CODER.md`, `PLAYBOOK.md`, `agent.md`.
- `python3 scripts/agent_memory.py recall --agent mpc-sample-coder <keywords>`
- Route: `python3 disklordz/integrations/agents/mpc_sample_coder.py --route "<task>"`

## Success criteria

- [ ] Correct lane skill applied (xpj vs xpm vs rust-mod vs lab vs sd)
- [ ] `mpc_sample_coder.py --self-test` passes when repo files changed
- [ ] Hardware claims tagged `[executed]` / `[assumed]`
- [ ] No firmware binaries or secrets in git

## Anti-patterns

- Mixing Sample `.xpj` with Live `MPCVObject` `.xpm` without naming target
- Multiple SD `autorun/` stacks on one card
- Vendoring AGPL Melodee into proprietary JUCE without compliance review
- Claiming KeygroupProgram Trap Lord export equals Melodee/Live import without migration