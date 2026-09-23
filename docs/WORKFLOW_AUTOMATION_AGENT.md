# Workflow Automation Agent

**Role:** Create, register, and schedule recurring automations for Instruments lanes (Gearmulator capture, mpc-agent exports, CI hygiene, team memory updates).

## Job catalog (source of truth)

Edit [.cursor/workflow-automation/jobs.yaml](../.cursor/workflow-automation/jobs.yaml). Each job has:

| Field | Meaning |
|-------|---------|
| `id` | Stable slug |
| `schedule` | Cron (UTC) or `manual` |
| `prompt` | Cloud Agent follow-up text when fired |
| `github_workflow` | Optional `.github/workflows/*` file |
| `owner` | `workflow-automation-agent` or human |

## Registered jobs (initial set)

| ID | Schedule | Action |
|----|----------|--------|
| `track-e-capture-smoke` | Mon 09:00 UTC | Run Wave909 + map finalize smoke (no ROM) |
| `track-e-osirus-fx-reminder` | Wed 09:00 UTC | Verify Osirus FX env + doc links if build exists |
| `mpc-agent-submodule-sync` | Fri 09:00 UTC | Remind push mpc-agent + bump submodule SHA |
| `team-memory-review` | 1st of month | Diff `TEAM_AGENT_MEMORY.md` vs last month |

## GitHub Actions

[`.github/workflows/workflow-automation-track-e.yml`](../.github/workflows/workflow-automation-track-e.yml) runs **capture smoke** on schedule + `workflow_dispatch`. It does **not** run JE-8086/Osirus (ROM secrets on maintainer machines only).

## Cloud Agent timers

The Workflow Automation Agent should register Cursor **subscribe_timer** entries matching `jobs.yaml` `cursor_timer` blocks (same `id` as name).

Example prompt on fire:

```text
Workflow job track-e-capture-smoke: run python3 scripts/gearmulator/render_multisample_dawdreamer.py
with WAVE-909, confirm instrument.map.json, update TEAM_AGENT_MEMORY changelog if broken.
```

## Creating a new automation

1. Add entry to `jobs.yaml`.
2. Add or extend a GitHub workflow if the job needs CI runners.
3. Call `subscribe_timer` with matching `name` + `cron`.
4. Append one line to [TEAM_AGENT_MEMORY.md](TEAM_AGENT_MEMORY.md) changelog.

## Integrations

| System | Use |
|--------|-----|
| Slack | CI notify — [REPO_AUTOMATION.md](REPO_AUTOMATION.md) |
| Airtable handoff | `.github/workflows/airtable-antigravity-handoff.yml` |
| RAG | [RAG_AND_INTELLIGENT_AUTOMATION.md](RAG_AND_INTELLIGENT_AUTOMATION.md) |

## Related

- [TEAM_AGENT_MEMORY.md](TEAM_AGENT_MEMORY.md)
- [CURSOR_AGENT_PLAYBOOK.md](CURSOR_AGENT_PLAYBOOK.md)
