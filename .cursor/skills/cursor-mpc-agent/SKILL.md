---
name: cursor-mpc-agent
description: "Delegate multisample → MPC Keygroup program work to the user's Cursor MPC-agent, with MPCTK as the export engine. Use after InstrumentMapSpec/WAV batch is ready."
---

# Cursor MPC-agent (specialist)

## Role

Turn **multisample sets** (many WAVs + zone metadata) into a playable **MPC Keygroup program** (`.xpj` + sample package). This is a **specialist** alongside the Instrument Sound Factory orchestrator.

**Your MPC-agent:** built in Cursor (custom agent / rules / skills on your machine or Cloud). It is **not** the same repo as [MPCTK](https://github.com/samuelfreemanjobs-hash/MPC-Sample-Toolkit), but should **call MPCTK** (CLI/GUI) or extend it for true keygroup layouts.

## Register (one-time)

Add to the WO or `disklordz/sound-factory/jobs/*.json`:

```json
"mpcAgent": {
  "cursorAgentName": "MPC-agent",
  "handoffDoc": "docs/INSTRUMENT_SOUND_FACTORY_AGENT.md#62-mpc-keygroup-programs"
}
```

When the MPC-agent repo or skill path is in git, link it here and in [AGENTS.md](../../../AGENTS.md).

## Handoff contract (orchestrator → MPC-agent)

Input bundle under `disklordz/sound-factory/out/<jobId>/`:

| File | Content |
|------|---------|
| `manifest.json` | Sample list, SHA-256, provenance |
| `instrument-map.json` | Zones: root, key range, vel range, tune cents, loop points |
| `samples/*.wav` | Rendered multisamples |
| `brief.md` | Sonic goal, reference report id (optional) |

**MPC-agent output:**

| File | Content |
|------|---------|
| `*.xpj` | MPC Sample project with **keygroup** program |
| `_[ProjectData]/` | Copied WAVs per MPCTK rules |
| `keygroup-report.json` | Pad/bank map, roots, QA notes |

## Runtime

| Where | MPC-agent runs |
|-------|----------------|
| **VS Code / local Cursor** | Preferred — MPCTK + hardware check |
| **Antigravity** | Same if MPCTK installed |
| **Cloud** | Prepare handoff only; spawn MPC-agent task locally or second Cloud run with `runtimeHint: local` |

## Orchestrator rule

If job pipeline includes `export.mpc_keygroup`, orchestrator **must not** skip unless `runtimeHint` is `cloud-only`; then it leaves `HANDOFF_MPC.md` with exact next prompt for MPC-agent.
