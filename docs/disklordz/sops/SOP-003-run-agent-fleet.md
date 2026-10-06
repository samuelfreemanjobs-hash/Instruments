# SOP-003 — Run the agent fleet (“get to work”)

## Purpose

Execute all **safe local** agent roles and refresh governance artifacts without waiting for CI.

## Local (developer or Cloud VM)

```bash
# Dev server on :3000 optional but enables full verify
./scripts/run-agent-fleet-now.sh
```

This runs:

- PM Agent / Workflow Automation sync
- Profit `--check`
- Prompt Coach chunk (`chunk_corpus.py`)
- Integration Health + Conversion QA smoke **if** `DISKLORDZ_URL` is set

Production smoke:

```bash
DISKLORDZ_URL=https://your-production-url ./scripts/run-agent-fleet-now.sh
```

## CI autopilot (on `main`)

| Workflow | When |
|----------|------|
| `agent-fleet-governance.yml` | Daily + pushes to fleet paths |
| `agent-fleet-execute.yml` | Weekdays |
| `agent-fleet-health.yml` | Daily |

Set GitHub secret **`DISKLORDZ_VERIFY_BASE_URL`** for remote verify in CI.

## Manual / stub agents

Billing, churn, analytics, marketing — import n8n JSON from `disklordz/agents/workflows/n8n/` and connect Stripe/Supabase in Zapier/MCP. Until then they remain documented but not unattended.

## Success criteria

- [ ] `fleet_execute` exits 0
- [ ] `verify-go-live.sh` passes against target URL
- [ ] No uncommitted drift after sync (CI governance job)
