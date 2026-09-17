# THOR × Taskmaster × DiskLordz — operating bridge

## Who is who

| Name | Where | Job |
|------|-------|-----|
| **THOR** | Slack `#executive-assistant` · Master Vault `executive-assistant/` | Executive Assistant — `/ea dashboard`, `/ea plan today`, cross-company P0 |
| **Taskmaster** | **Alias:** DiskLordz **Workflow Automation Engineer** + Airtable automation | Turns Work Orders → GitHub issues, CI signals, daily rollups |
| **Taskhelper** | **Alias:** **Audio PM** skill | WIP limits, WO reconciliation, escalations |
| **Cursor Cloud Agent** | This repo | Implements `[Plugin][CharacterFM]` / JD WOs, opens PRs |

THOR does **not** write C++. Taskmaster does **not** replace Product Orchestrator on *what* to build.

## Daily rhythm (every business day)

```text
06:00  Taskmaster (automation) — sync open WOs → GitHub; post CI summary #disklordz-ci
08:00  Taskhelper (Audio PM agent run) — WIP check (max 2 JUCE WOs), blockers → Airtable
09:00  THOR — `/ea dashboard` includes DiskLordz section (see yaml below)
EOD    Marketing Growth — customer-facing delta paragraph → #disklordz-dev
EOD    Cloud Agent — engineering delta + PR links → #disklordz-dev
```

## DiskLordz section for THOR dashboard

Add to Master Vault `executive-assistant/dashboard-config.yaml` (maintained there; mirror here):

```yaml
disklordz:
  repo: samuelfreemanjobs-hash/Instruments
  slack_dev_channel: "#disklordz-dev"
  p0:
    - "Character FM Milestone 1 WO queue in Airtable"
    - "Merge agent-os + CHARACTER_FM docs PR"
  metrics:
    - "Last green build on main"
    - "Open Plugin WOs count (max 2)"
    - "Last Marketing Growth post date"
```

## Slack commands (human)

| Command | Agent |
|---------|--------|
| `@Cursor [Plugin][CharacterFM] …` | Cloud implementer |
| `/ea dashboard` | THOR |
| `@Audio PM reconcile WOs` | Taskhelper (Audio PM skill run) |

## Escalation

1. Engineering blocked > 24h → Audio PM tags Orchestrator + THOR follow-up  
2. Customer-facing copy needed → Marketing Growth WO  
3. Irreversible commercial action → Creative Director only  

## What was missing “last night”

If no rollup appeared, run **this checklist manually once**, then automate:

1. `cmake --build build` + `tests/golden/verify_golden.sh` on `main`  
2. Post results to `#disklordz-dev` (Cloud Agent or webhook)  
3. Audio PM: export open WOs from Airtable  
4. THOR: `/ea plan today` including DiskLordz P0  

This repo now ships the **program docs**; THOR yaml lives in **Revenue-Systems-Engineer-Master-Vault** until synced.
