# Airtable → Cursor Cloud handoff (V Voyager / Plugin Factory)

## Purpose

Queue **Agent Work Orders** for **`cursor-v-voyager`** without manual copy-paste. Airtable automation fires GitHub `repository_dispatch`; the workflow writes **`CO-*.json`** inbox files for Cloud Agents to pick up after `git pull`.

## Build & run

```bash
cd disklordz/automation
cp .env.example .env   # AIRTABLE_*
python3 scripts/wo_to_cursor_cloud_handoff.py --work-order-id WO-2026-VVOY-002 --dry-run
```

Manual GitHub:

```bash
gh workflow run airtable-cursor-cloud-handoff.yml -f work_order_id=WO-2026-VVOY-002
```

## Data flow

```text
Airtable WO (owner cursor-v-voyager) → status Ready for Cursor Cloud
    → repository_dispatch airtable-cursor-cloud-handoff
    → wo_to_cursor_cloud_handoff.py
    → disklordz/cursor-cloud/inbox/CO-*.json on branch
    → Slack (optional) → Cloud Agent reads WO + runs on branch
```

## Key modules

| Path | Role |
|------|------|
| `handoff.schema.json` | JSON shape |
| `AGENT_INSTRUCTIONS.md` | Cloud Agent system context |
| `inbox/` | Pending WOs for Cursor Cloud |
| `../automation/scripts/wo_to_cursor_cloud_handoff.py` | Airtable fetch + write CO file |

## Related docs

- PM: [docs/V_VOYAGER_PM.md](../../docs/V_VOYAGER_PM.md)
- Seed data: [disklordz/airtable/seed/v-voyager-2026.json](../airtable/seed/v-voyager-2026.json)
