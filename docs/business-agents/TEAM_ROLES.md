# Business team roles (no API)

These three roles are **members of the agent team**, same as factory or SaaS implementers. They use **Cursor Cloud** or **Claude Code** with repo prompts — **not** repo API keys or `business_agent.py --run`.

## Roster

| Role | ID | Primary runtime | Delivers | Human owner |
|------|-----|-----------------|----------|-------------|
| **SKU research** | `business-sku-research` | Cursor Cloud (Planner WOs) | `brief.json` | Business Planner |
| **Marketing content** | `business-content` | Cursor or Claude | `content-drafts/*.json` | Marketing (you publish) |
| **Ship evaluator** | `business-ship-eval` | Cursor Cloud (PR / release WOs) | `ship-evaluations/*-eval.json` | Owner / release |

Registry: [agent-registry.json](../agent-registry.json) → `businessAgents`.

## How to invoke (Cloud Agent task)

Use [CURSOR_AGENT_PLAYBOOK.md](../CURSOR_AGENT_PLAYBOOK.md) template and add:

```markdown
## Business role
Act as **business-sku-research** (or content / ship-eval).

## Read
- docs/business-agents/prompts/<role>-system.md
- Intake: business-agents/sku-briefs/<id>/intake.yaml (if SKU)

## Write
- Save JSON artifact under business-agents/ per TEAM_ROLES.md
- Run: python3 scripts/business-agents/business_agent.py <role> validate --file …
- Commit + push with PR evidence
```

## Claude Code

Open repo root; read [.claude/agents/business-sku-research/AGENT.md](../../.claude/agents/business-sku-research/AGENT.md) (and siblings). Same artifacts and validate step.

## CLI (scaffold + validate only)

```bash
python3 scripts/business-agents/business_agent.py sku init --product-id DL-… --name "…"
python3 scripts/business-agents/business_agent.py sku validate --file …/brief.json
python3 scripts/business-agents/business_agent.py check-pr
```

`prompt` subcommand prints context for **you** when working in IDE without Cloud — still no API in repo.

## Handoff to implementers

| After | Next agent |
|-------|------------|
| `planner_recommendation.decision: proceed` | Cursor factory on `repoPath` from [agent-registry.json](../agent-registry.json) |
| Content draft ACCEPT (human) | Owner pastes to site/store; optional SaaS agent for `disklordz/website/` |
| Ship eval ACCEPT | Human DAW/deploy gates still required per [GATES.md](../plugin-factory/GATES.md) |
