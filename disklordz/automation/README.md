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

## Daily Factory DSP Cloud Agent (WO-SAAS-018)

**Schedule:** GitHub Action [`.github/workflows/disklordz-factory-daily.yml`](../../.github/workflows/disklordz-factory-daily.yml) — 12:30 UTC daily.

1. Runs `npm run factory:dsp-regression` + build + lint on `disklordz/website`.
2. If `CURSOR_API_KEY` is set, launches a Cursor Cloud Agent with the prompt in [`prompts/factory-daily-improvement.md`](prompts/factory-daily-improvement.md).

```bash
# Full local pipeline (same as CI + optional agent)
bash scripts/run-factory-daily.sh
# or from repo root: ./scripts/disklordz-factory-daily.sh

# Diagnostics
bash scripts/check-factory-daily-setup.sh

# Manual GitHub trigger
gh workflow run disklordz-factory-daily.yml
gh workflow run disklordz-factory-daily.yml -f theme=loops_patterns

# Dry-run agent prompt
bash scripts/trigger_cursor_factory_daily_agent.sh --dry-run
```

**Secrets:** `CURSOR_API_KEY` (optional), `SLACK_WEBHOOK_URL` (optional summary).

PM + persona: [docs/DISKLORDZ_FACTORY_DAILY_AGENT.md](../../docs/DISKLORDZ_FACTORY_DAILY_AGENT.md).

## Business agents (WO-SAAS-019–023)

Full catalog: [docs/DISKLORDZ_BUSINESS_AGENTS.md](../../docs/DISKLORDZ_BUSINESS_AGENTS.md).

```bash
./scripts/disklordz-business-agents.sh check
bash scripts/run-saas-ops-daily.sh --skip-agent
bash scripts/check-billing-integrity.sh
echo "stripe webhook" | node scripts/pm-router-classify.mjs
```

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
