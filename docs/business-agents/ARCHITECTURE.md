# Business agents — architecture

## Purpose

Three **teammate roles** on the same roster as factory and SaaS agents. **Cursor Cloud** and **Claude Code** read prompts and **commit JSON artifacts** — the repo CLI does **not** call LLM APIs.

| Teammate | Pattern | Artifact |
|----------|---------|----------|
| **business-sku-research** | #5 / #9 | `business-agents/sku-briefs/<id>/brief.json` |
| **business-content** | #2 | `business-agents/content-drafts/*.json` |
| **business-ship-eval** | #8 | `business-agents/ship-evaluations/*-eval.json` |

**Team roster:** [TEAM_ROLES.md](TEAM_ROLES.md) · **Cursor rule:** [`.cursor/rules/business-agents-team.mdc`](../../.cursor/rules/business-agents-team.mdc)

## Build & run (CLI — no API)

```bash
python3 scripts/business-agents/business_agent.py sku init --product-id DL-… --name "…"
python3 scripts/business-agents/business_agent.py team --role sku-research --request "Complete brief for …"
python3 scripts/business-agents/business_agent.py sku validate --file …/brief.json
python3 scripts/business-agents/business_agent.py check-pr
```

Assign the printed **team** task to Cloud Agent, or open Claude with `.claude/agents/business-*/AGENT.md`.

## Data flow

```text
Owner / Planner idea
  → sku init + intake.yaml
  → Cloud Agent as business-sku-research → brief.json + validate
  → Factory agent on repoPath
  → business-content → draft JSON
  → business-ship-eval → eval JSON → human ship
```

## Key modules

| Path | Role |
|------|------|
| `docs/business-agents/prompts/` | System prompts |
| `scripts/business-agents/business_agent.py` | Scaffold, validate, team task template |
| `.claude/agents/business-*/` | Claude Code teammate entry |
| `integrations/automation/business-agents.yaml` | Reminders (no API cron) |

## Related

- [AGENTIC_OPERATING_MODEL.md](../AGENTIC_OPERATING_MODEL.md)
- [agent-registry.json](../agent-registry.json) → `businessAgents`
