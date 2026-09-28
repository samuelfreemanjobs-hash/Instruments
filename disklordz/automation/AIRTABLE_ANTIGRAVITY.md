# Airtable → Antigravity handoff automation

## Prerequisites

| System | Config |
|--------|--------|
| **Airtable** | `Agent Work Orders` with `work_order_id`, `title`, `acceptance_criteria`, `owner_agent`, `status` |
| **GitHub** | Secrets `AIRTABLE_API_KEY`, `AIRTABLE_BASE_ID`; optional `SLACK_WEBHOOK_ANTIGRAVITY_URL` |
| **Antigravity PC** | `sync-handoff.ps1` after `git pull` |

Optional Airtable field: `antigravity_handoff_id` (single line text) — patched when using `--patch-airtable` in CI.

## Flow

```text
Planner sets WO → owner antigravity-hise / title [Plugin][HISE]
    → Airtable automation fires repository_dispatch
    → GitHub Action runs wo_to_antigravity_handoff.py
    → commits inbox/HO-*.json to main
    → antigravity-inbox-slack.yml notifies Slack
    → Windows: git pull + sync-handoff.ps1
```

## Airtable automation (sketch)

1. Trigger: When `status` = `Ready for Antigravity` (or button field).
2. Action: Webhook to GitHub `repository_dispatch` (see README).
3. Filter: `owner_agent` = `antigravity-hise` OR `title` contains `[Plugin][HISE]`.

## Manual test

```bash
gh workflow run airtable-antigravity-handoff.yml -f work_order_id=WO-2026-HISE-001
```

Use seed WO from `disklordz/airtable/seed/hise-sketch-lane-2026.json` after importing to base.
