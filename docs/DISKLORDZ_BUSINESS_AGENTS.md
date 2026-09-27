# Disklordz business agents (WO-SAAS-018–028)

Automated **Cloud Agent personas** + **GitHub Actions** to run the SaaS business: sound, uptime, billing, growth, presets, routing, support, analytics, async jobs, and QA.

**Full setup:** [DISKLORDZ_COMPLETE_SETUP.md](DISKLORDZ_COMPLETE_SETUP.md)

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
| **024** | [Customer Success](../.cursor/agents/disklordz-customer-success.md) | Tue 15:00 UTC | [Support runbook](DISKLORDZ_SUPPORT_RUNBOOK.md) | `disklordz-support-weekly.yml` |
| **025** | [Analytics Funnel](../.cursor/agents/disklordz-analytics-funnel.md) | Thu 15:00 UTC | `GET /api/ops/analytics-summary` + `OPS_API_KEY` | `disklordz-analytics-weekly.yml` |
| **026** | [Async Jobs](../.cursor/agents/disklordz-async-jobs.md) | Wed 16:00 UTC | loop/SFX + `GET /api/jobs/:id` | `disklordz-async-jobs-audit.yml` |
| **027** | [Audio QA / Golden](../.cursor/agents/disklordz-audio-qa.md) | Sat 15:00 UTC | `factory-golden-lanes.json` in regression | `disklordz-audio-qa-weekly.yml` |
| **028** | [Competitive Intel](../.cursor/agents/disklordz-competitive-intel.md) | 1st of month 15:00 UTC | `docs/DISKLORDZ_MARKET_INTELLIGENCE.md` | `disklordz-competitive-intel-monthly.yml` |

Detail for 018: [DISKLORDZ_FACTORY_DAILY_AGENT.md](DISKLORDZ_FACTORY_DAILY_AGENT.md).

## Product features (built for agents)

| Feature | WO | Code |
|---------|-----|------|
| Generation analytics events | 025 | `generation_events` + `logGenerationEvent` |
| Async loop/SFX jobs | 026 | `POST /api/generate` + `async: true`, `/api/jobs/[id]` |
| Golden lane regression | 027 | `factory-golden-lanes.json` |

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
