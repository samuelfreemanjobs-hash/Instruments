# VST Plugin Factory — agent schedule (Workflow Automation Agent)

Designed for **workflow-automation-agent**: GitHub cron, Airtable buttons, and optional **Cursor Automations** / **subscribe_timer** loops on long-running Cloud Agent threads.

All times **UTC**. Adjust for your team timezone in Cursor Automations UI; keep GitHub cron in UTC.

## Weekly calendar

| When (UTC) | Trigger | Actor | Action |
|------------|---------|-------|--------|
| **Mon 09:00** | GitHub cron `health` | workflow-automation-agent | Run plugin CI health job; upload summary artifact |
| **Mon 09:30** | Cursor Automation (recommended) | cursor-vst-plugin-factory | Read health artifact; if fail → draft fix WO for pm-agent |
| **Daily 11:00** | `nightly-qa.yml` | GitHub | Full monorepo QA (existing) |
| **Tue–Thu** | Event-driven only | cursor-vst-plugin-factory | Execute `[Plugin][JUCE]` WOs from inbox |
| **Fri 17:00** | GitHub cron `doc-sync` | workflow-automation-agent | Open/update doc-sync checklist (ARCHITECTURE vs tree) |
| **Fri 17:30** | Cursor Automation (optional) | cursor-vst-plugin-factory | 30m doc pass if checklist non-empty |

## Event-driven (always on)

| Event | Automation | Result |
|-------|------------|--------|
| Airtable WO → `owner_agent = cursor-vst-plugin-factory` | `repository_dispatch`: `airtable-vst-factory-handoff` | `disklordz/vst-factory/inbox/HO-*.json` committed |
| Push to `cursor/*` with plugin paths | `build.yml` / PR CI | `cmake` required check |
| Merge to `main` | Nightly + PR CI | Regression signal |

## Cursor Automation templates

Create automations in Cursor (Cloud Agent API or dashboard) pointing at repo **Instruments**, branch **main** unless handoff specifies otherwise.

### 1. Monday health triage (cron `30 9 * * 1`)

```markdown
You are cursor-vst-plugin-factory on a scheduled health triage.

1. Download the latest artifact from workflow run "VST Plugin Factory schedule" job "plugin-ci-health" (or run locally: python3 vst-testing-ops/run_business.py --profile ci).
2. If failed: summarize failures from vst-testing-ops/error_log.txt; do NOT fix unless a WO exists. Post a comment template for pm-agent to open WO-2026-JUCE-HEALTH-XXX.
3. If passed: reply "Plugin factory health green" only.
```

### 2. Friday doc sync (cron `30 17 * * 5`)

```markdown
You are cursor-vst-plugin-factory. Compare root ARCHITECTURE.md product table to CMakeLists.txt targets and docs/*/ARCHITECTURE.md presence. List mismatches in a short markdown checklist. If empty, no PR. If non-empty, open draft PR updating docs only.
```

### 3. Inbox consumer (manual or webhook on push to `disklordz/vst-factory/inbox/`)

```markdown
Read the newest open HO-*.json in disklordz/vst-factory/inbox/. Execute the WO using docs/VST_PLUGIN_FACTORY_AGENT.md. Branch from work_order.branch. Mark handoff notes in PR body.
```

## Subscription timers (in-conversation)

For an active Cloud Agent already working a plugin WO, **workflow-automation-agent** may register:

| Timer name | Cron | Prompt |
|------------|------|--------|
| `vst-factory-ci-wait` | (use PR subscription instead) | Prefer `subscribe_github_ci` on the PR branch |
| `vst-factory-weekly-health` | `0 9 * * 1` | Run health triage template above |

Use [subscribe skill](https://cursor.com/docs) / `cursor-subscriptions-subscribe_timer` only when event tools do not apply.

## Capacity (Factory Manager)

- Max **2** concurrent `[Plugin][JUCE]` WOs **In Progress** on Cursor.
- Scheduled health/doc jobs **do not** count as WOs unless pm-agent promotes a failure to a full WO.

## Secrets & integrations

| Secret | Use |
|--------|-----|
| `AIRTABLE_API_KEY`, `AIRTABLE_BASE_ID` | WO handoff scripts |
| `SLACK_WEBHOOK_URL` | Optional health notifications (same as CI Slack) |
| Cursor Cloud API key | Dashboard automations (not stored in git) |

Wire-up details: [disklordz/automation/VST_PLUGIN_FACTORY.md](../disklordz/automation/VST_PLUGIN_FACTORY.md).
