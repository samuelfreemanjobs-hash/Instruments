---
name: pm-airtable-factory-trigger
description: "Start factory runs from PM Agent / Airtable work orders or from manual Cursor Agent tasks. Use when planning or executing WO-driven batch jobs."
---

# PM Agent / Airtable → factory trigger

## Locked policy (product owner)

Runs start **either**:

1. **You** — Cloud or VS Code Agent with a goal + optional `disklordz/sound-factory/jobs/<id>.json`, or  
2. **PM Agent + Airtable** — row planned in Disklordz OS → implementation agent kicked automatically or via one-click handoff.

Both are first-class; no preference required at job time.

## Airtable → agent flow

```text
PM Agent (or human Planner) creates WO row
  work_order_id, title, acceptance_criteria, assignee_lane
  factory_pipeline (optional JSON), presetId, prompt, generationSpec
        │
        ▼
GitHub issue (title contains WO-…)     [existing Disklordz pattern]
        │
        ├── Cloud Agent API / Cursor automation (Phase 2)
        └── Manual: "Run instrument-orchestrator for WO-SF-xxx"
        │
        ▼
Job file: disklordz/sound-factory/jobs/<work_order_id>.json
        │
        ▼
Instrument orchestrator (+ MPC-agent step if keygroup)
```

## WO fields to add (Airtable / PM Agent)

| Field | Example |
|-------|---------|
| `work_order_id` | `WO-SF-012` |
| `assignee_lane` | `instrument-factory` \| `mpc-agent` \| `split` |
| `factory_pipeline` | JSON array of steps (see enterprise doc) |
| `runtimeHint` | `cloud` \| `local` \| `split` |
| `reference` | `{ "type": "youtube", "url": "...", "segmentSec": 60 }` |

## Zapier / MCP

- **Read** Airtable WO: OK without extra confirmation.  
- **Write** (status → In Progress / Done): confirm with user per security baseline unless automation is pre-approved for factory lane.

## PM Agent prompt snippet

```markdown
Create WO-SF-xxx for [SKU brief]. Set assignee_lane=split.
Acceptance: manifest + keygroup .xpj via MPC-agent.
Generate matching jobs/example-drum-batch.json shape under sound-factory/jobs/.
Kick Cursor Cloud Agent on branch cursor/...
```
