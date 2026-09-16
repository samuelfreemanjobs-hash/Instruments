# DiskLordz Agent Team

Orchestration model for Cursor Cloud Agents and local chat. **Audio PM** schedules and tracks; **Workflow Automation Engineer** closes the loop on glue code and integrations; specialist agents execute work orders.

## Invocation

| Agent | How to run |
|-------|------------|
| Audio PM | Open chat: *"Use the disklordz-audio-pm skill"* or `@disklordz-audio-pm` if indexed |
| Workflow Automation Engineer | *"Use the disklordz-workflow-automation-engineer skill"* |
| Any specialist | Audio PM creates a Work Order with `assigned_agent`; run that skill in a Cloud Agent job |

Skills live in `.cursor/skills/`.

## Hierarchy

```text
Creative Director (human)
        │
        ▼
   Audio PM Agent ───────────────► Airtable (system of record)
        │
        ├──► Workflow Automation Engineer (autopilot backlog)
        │
        └──► Product Factory (Phase 2+)
                 ├── Plugin division agents
                 └── Sample division agents
```

## Roster

| # | Agent | Skill (planned) | Phase |
|---|-------|-----------------|-------|
| — | **Audio PM** | `disklordz-audio-pm` | 0 |
| — | **Workflow Automation Engineer** | `disklordz-workflow-automation-engineer` | 0 |
| 1 | Audio Product Director | `disklordz-product-director` | 1 |
| 2 | Plugin Architect | `disklordz-plugin-architect` | 1 |
| 3 | JUCE/C++ Engineer | `disklordz-juce-engineer` | 1 |
| 4 | DSP Engineer | `disklordz-dsp-engineer` | 1 |
| 5 | Plugin UI Engineer | `disklordz-plugin-ui` | 1 |
| 6 | Plugin QA | `disklordz-plugin-qa` | 1 |
| 7 | Drum Sound Designer | `disklordz-sound-designer` | 1 |
| 8 | Sample Processing | `disklordz-sample-processing` | 1 |
| 9 | Kit Architect | `disklordz-kit-architect` | 1 |
| 10 | MPC Agent | `disklordz-mpc` | 2 |
| 11 | Product Factory | `disklordz-product-factory` | 2 |
| 12 | Release Agent | `disklordz-release` | 1 |

## Delegation rules

1. **No orphan work** — every agent task has an Airtable Work Order linked to a Project.
2. **PRs link back** — `github_pr` on the WO before marking `Review`.
3. **PM closes loops** — when an agent finishes, PM updates status and queues the next WO.
4. **Automation Engineer owns integrations** — new Zapier/n8n flows get an Automations row.
5. **Human gate** — only Creative Director moves Product to `Released`.

## Default repos (current)

| Division | Repo / path |
|----------|-------------|
| Plugins | `Instruments` / `MyFirstPlugin` |
| Company OS | `disklordz/` |
| Future split | `plugin-engine`, `drum-factory`, etc. per MASTER_PLAN |
