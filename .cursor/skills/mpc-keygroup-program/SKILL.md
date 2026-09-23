---
name: mpc-keygroup-program
description: "Multisample sets → MPC Keygroup programs via Cursor MPC-agent + MPCTK. Use for chromatic/stacked zones across the keyboard, not one-shot drum pads only."
---

# MPC Keygroup program (multisamples)

## Goal

**Multisample WAV set + zone map → MPC Keygroup program** (playable `.xpj` on MPC Sample / MPC Software workflow you validate).

## Agents

| Agent | Responsibility |
|-------|----------------|
| **Instrument orchestrator** | Renders WAVs, `instrument-map.json`, manifest |
| **Cursor MPC-agent** (yours) | Keygroup layout, MPCTK invocation, pad/bank QA |
| **MPCTK** | `.xpj` + `_[ProjectData]` package engine |

Read [cursor-mpc-agent](../cursor-mpc-agent/SKILL.md) for handoff files.

## MPCTK

https://github.com/samuelfreemanjobs-hash/MPC-Sample-Toolkit — chromatic/keygroup generation, WAV injection, hardware-validated.

Extend MPCTK or MPC-agent when stock chromatic generator is not enough for **true keygroup** velocity layers / overlapping zones.

## Pipeline step

```json
{ "step": "export.mpc_keygroup", "delegate": "cursor-mpc-agent", "required": true }
```

**Runtime:** local VS Code or Antigravity (not Cloud-only without handoff).

## Install (local)

```bash
git clone https://github.com/samuelfreemanjobs-hash/MPC-Sample-Toolkit.git
cd MPC-Sample-Toolkit && python3 -m venv .venv && source .venv/bin/activate
python -m pip install -e .
```

Confirm before overwriting user MPC project directories.
