# Agent workflows & PM ADD

## Purpose

**PM Agent** publishes the fleet at repo root (`DISKLORDZ_AGENTS.md`, `.github/agents/fleet.json`) and runs governance CI. **Workflow Automation** maps each profit agent to a concrete trigger (GitHub Actions, Inngest, API route, n8n stub, or manual SOP) and publishes the **PM ADD** roster so product and eng see who does what for Disklordz revenue.

## Build & run

```bash
./scripts/sync-disklordz-agent-fleet.sh
```

CI: **Agent fleet governance** (PM Agent), **Agent fleet execute** (weekday roles), **Scaffold agent workflows**, **Agent fleet health**.

## Data flow

```text
profit/registry.json + scaffold_workflows.py (ANNOUNCE + WORKFLOWS)
  → PM_ADD.md (human roster)
  → manifest.json (machine index)
  → agents/<id>/automation.yaml (per-agent runbook)
  → .github/workflows/* + n8n/* stubs (execution)
```

## Key modules

| Path | Role |
|------|------|
| [`PM_ADD.md`](PM_ADD.md) | Agent self-announcements + automation pointers |
| [`manifest.json`](manifest.json) | JSON roster for tooling |
| [`scaffold_workflows.py`](scaffold_workflows.py) | Source of announcements and workflow map |
| [`agents/<id>/automation.yaml`](agents/) | Per-agent platform, trigger, steps |
| [`n8n/`](n8n/) | Importable workflow stubs (billing, churn, analytics, marketing) |
| [`scripts/`](scripts/) | Shell helpers (release notes, social clip stub) |

## Threading / realtime

Workflow scaffolds are offline/CI-only. Inngest and `/api/generate` paths remain subject to website realtime rules (no blocking audio thread — N/A here).

## Extension points

1. Add `ANNOUNCE` + `WORKFLOWS` entries in `scaffold_workflows.py` for new agents in `_specs.json`.
2. Run `scaffold_workflows.py` and commit `PM_ADD.md` + `automation.yaml` diffs.
3. Replace stubs with live n8n/Stripe/Slack credentials per `disklordz/website/DEPLOY.md`.

## Related docs

- [`../ARCHITECTURE.md`](../ARCHITECTURE.md) — profit agent fleet
- [`../../integrations/ARCHITECTURE.md`](../../integrations/ARCHITECTURE.md) — verify-integrations
- [docs/DISKLORDZ_PROFIT_AGENTS.md](../../../docs/DISKLORDZ_PROFIT_AGENTS.md)
