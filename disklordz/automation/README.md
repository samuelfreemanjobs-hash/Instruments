# Disklordz automation

## Antigravity handoff from Airtable

```bash
cd disklordz/automation
cp .env.example .env   # fill AIRTABLE_*
python3 scripts/wo_to_antigravity_handoff.py --work-order-id WO-2026-HISE-001
```

Creates `disklordz/antigravity/inbox/HO-*.json`. Commit + push, or use GitHub Action below.

### Airtable button → GitHub

Automation → Run script or Zapier → `POST` GitHub **repository_dispatch**:

```http
POST https://api.github.com/repos/samuelfreemanjobs-hash/Instruments/dispatches
Authorization: Bearer <GITHUB_PAT with repo scope>
Accept: application/vnd.github+json

{
  "event_type": "airtable-antigravity-handoff",
  "client_payload": {
    "work_order_id": "WO-2026-HISE-001",
    "branch": "main"
  }
}
```

Workflow: [`.github/workflows/airtable-antigravity-handoff.yml`](../../.github/workflows/airtable-antigravity-handoff.yml)

Secrets in GitHub: `AIRTABLE_API_KEY`, `AIRTABLE_BASE_ID`

See [AIRTABLE_ANTIGRAVITY.md](AIRTABLE_ANTIGRAVITY.md).

## Slack on new inbox files

When `disklordz/antigravity/inbox/HO-*.json` is pushed to `main`, workflow posts to Slack.

Secret: `SLACK_WEBHOOK_URL` (or `SLACK_WEBHOOK_ANTIGRAVITY_URL` for a dedicated channel webhook).
