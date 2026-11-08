# Airtable → Cursor Cloud handoff (V Voyager)

## Prerequisites

| System | Config |
|--------|--------|
| **Airtable** | `Agent Work Orders` with `work_order_id`, `title`, `acceptance_criteria`, `owner_agent`, `status` |
| **GitHub** | Secrets `AIRTABLE_API_KEY`, `AIRTABLE_BASE_ID` |
| **Agent** | `cursor-v-voyager` on Cursor Cloud (repo `samuelfreemanjobs-hash/Instruments`) |

Optional Airtable field: `cursor_cloud_handoff_id` (single line text) — patched when using `--patch-airtable` in CI.

## Flow

```text
Planner sets WO → owner cursor-v-voyager / title [Plugin][Factory]
    → Airtable automation fires repository_dispatch (airtable-cursor-cloud-handoff)
    → GitHub Action runs wo_to_cursor_cloud_handoff.py
    → commits disklordz/cursor-cloud/inbox/CO-*.json
    → Cursor Cloud Agent picks up WO (manual or Dashboard automation)
```

## Airtable automation (sketch)

1. **Trigger:** When `status` = `Ready for Cursor Cloud` (create in base).
2. **Action:** Webhook to GitHub `repository_dispatch` (see [README.md](README.md)).
3. **Filter:** `owner_agent` = `cursor-v-voyager` OR `title` contains `[Plugin][Factory]`.

## Scheduled automations (GitHub — no Airtable required)

| Workflow | Cron (UTC) | Purpose |
|----------|------------|---------|
| [scheduled-plugin-factory-qa.yml](../../.github/workflows/scheduled-plugin-factory-qa.yml) | `0 12 * * 1,3,5` | `factory.sh release` + **V Voyager.vst3** |
| [nightly-qa.yml](../../.github/workflows/nightly-qa.yml) | `0 11 * * *` | Full monorepo QA (`--profile full`, includes factory) |

## Manual test

```bash
gh workflow run airtable-cursor-cloud-handoff.yml -f work_order_id=WO-2026-VVOY-002
```

Import seed rows: [disklordz/airtable/seed/v-voyager-2026.json](../airtable/seed/v-voyager-2026.json)
