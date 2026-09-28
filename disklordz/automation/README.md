# Disklordz automation

## Seed PM bundle (Products + Projects + Work Orders)

```bash
export AIRTABLE_API_KEY='pat...'
export AIRTABLE_BASE_ID='app...'   # Disklordz OS base

python3 disklordz/automation/scripts/seed_airtable_bundle.py --dry-run
python3 disklordz/automation/scripts/seed_airtable_bundle.py
```

Seed: [`airtable/seed/plugin-tracks-2026.json`](airtable/seed/plugin-tracks-2026.json) — **Junova-X**, **NovaDrum**, **Drum SaaS** products; projects; WO-2026-001 … 003.

Requires **Product Families** row `DL-FAMILY-DIGITAL-SAMPLER` (from full `bootstrap.json` on PR #9) or create that family manually first.

Work orders only (legacy): `seed_work_orders.py` + `work-orders-junova-2026.json`.

Then queue GitHub issues (when workflow on `main`):

```bash
gh workflow run airtable-work-order-to-github.yml -f work_order_id=WO-2026-001
```

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

When `disklordz/antigravity/inbox/HO-*.json` is pushed to **`main`**, workflow posts to **#disklordz-dev** (via your webhook).

Secrets:

- `SLACK_WEBHOOK_ANTIGRAVITY_URL` (preferred) or `SLACK_WEBHOOK_URL`
- Optional `SLACK_MENTION_USER_ID` — Slack member id (`U…`) so the message opens with `<@you>`

```bash
./scripts/setup-disklordz-integrations.sh slack-antigravity \
  --webhook-url 'https://hooks.slack.com/services/...' \
  --mention-user-id 'U0123456789'
```

Find your member id: Slack profile → ⋮ → **Copy member ID** (requires Slack admin setting “Show member IDs” in workspace settings).
