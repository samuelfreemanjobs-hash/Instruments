# Disklordz business agents (WO-SAAS-018–023)

Automated **Cloud Agent personas** + **GitHub Actions** to run the SaaS business: sound, uptime, billing, growth, presets, and routing.

**Shared setup:** GitHub secret `CURSOR_API_KEY` + Cursor Cloud Agents connected to this repo. Without the key, **check scripts and CI still run**; agent launch is skipped.

## Agent catalog

| WO | Agent | Schedule | Check / run locally | Workflow |
|----|--------|----------|---------------------|----------|
| **018** | [Factory DSP Engineer](../.cursor/agents/disklordz-factory-dsp-engineer.md) | Daily 12:30 UTC | `./scripts/disklordz-factory-daily.sh` | `disklordz-factory-daily.yml` |
| **019** | [SaaS Ops Guardian](../.cursor/agents/disklordz-saas-ops-guardian.md) | Daily 13:00 UTC | `bash disklordz/automation/scripts/run-saas-ops-daily.sh --skip-agent` | `disklordz-saas-ops-daily.yml` |
| **020** | [Billing Integrity](../.cursor/agents/disklordz-billing-integrity.md) | Mon 14:00 UTC | `bash disklordz/automation/scripts/check-billing-integrity.sh` | `disklordz-billing-weekly.yml` |
| **021** | [Growth & Lane Marketing](../.cursor/agents/disklordz-growth-lane-marketing.md) | Wed 14:00 UTC | `bash disklordz/automation/scripts/trigger_growth_agent.sh --dry-run` | `disklordz-growth-weekly.yml` |
| **022** | [PM / WO Router](../.cursor/agents/disklordz-pm-wo-router.md) | On dispatch | `echo "stripe bug" \| bash disklordz/automation/scripts/trigger_pm_router_agent.sh` | `disklordz-pm-router.yml` |
| **023** | [Preset Lane Curator](../.cursor/agents/disklordz-preset-lane-curator.md) | Fri 14:00 UTC | `node disklordz/automation/scripts/check-preset-lanes.mjs` | `disklordz-preset-curator-weekly.yml` |

Detail for 018: [DISKLORDZ_FACTORY_DAILY_AGENT.md](DISKLORDZ_FACTORY_DAILY_AGENT.md).

## One-shot: run all static checks

```bash
./scripts/disklordz-business-agents.sh check
```

## PM router dispatch (Airtable / Zapier / Slack)

```http
POST https://api.github.com/repos/samuelfreemanjobs-hash/Instruments/dispatches
{
  "event_type": "disklordz-pm-router",
  "client_payload": {
    "intake": "Users report Stripe checkout succeeds but Pro not active",
    "mode": "auto",
    "ref": "main"
  }
}
```

Classifier-only:

```bash
echo "phonk kick sounds weak" | node disklordz/automation/scripts/pm-router-classify.mjs
```

## Secrets

| Secret | Agents |
|--------|--------|
| `CURSOR_API_KEY` | All Cloud Agent launches |
| `DISKLORDZ_URL` | 019 live `verify:go-live` in CI |
| `SLACK_WEBHOOK_URL` | Optional summaries (factory daily) |

See [DISKLORDZ_GO_LIVE_SECRETS.md](DISKLORDZ_GO_LIVE_SECRETS.md).

## Branch naming

`cursor/<lane>-<topic>-4170` — e.g. `cursor/saas-ops-health-4170`, `cursor/billing-webhook-idempotency-4170`.
