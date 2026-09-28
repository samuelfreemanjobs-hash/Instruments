# 15 profit agents (implemented fleet)

Elite agent file trees live under **`disklordz/agents/profit/<id>/`** — each folder includes:

| File | Role |
|------|------|
| `agent.md` | Frontmatter + system prompt (Claude Code agent definition) |
| `skill.md` | Two-phase skill (when_to_use, paths) |
| `subagents.md` | Coordinator / explore / verify / implement |
| `soul.md` | Non-negotiable principles |
| `hooks.md` | PreToolUse, Stop, SessionStart |
| `memory.md` | File-tier MEMORY layout |
| `tools.md` | Allowlist + MCP + concurrency |
| `loop.md` | AsyncGenerator query() SOP |

Registry: [`disklordz/agents/profit/registry.json`](../disklordz/agents/profit/registry.json)

Copilot/Cursor skills (published from skill.md): `.github/skills/disklordz-<id>/SKILL.md`

## Regenerate

```bash
python3 disklordz/agents/profit/scaffold_agents.py --publish-skills
python3 disklordz/agents/profit/scaffold_agents.py --check
```

## The 15 agents

| # | ID | Title | Entry |
|---|-----|-------|-------|
| 1 | conversion-qa | Conversion QA | [agent.md](../disklordz/agents/profit/conversion-qa/agent.md) |
| 2 | billing-ops | Billing Ops | [agent.md](../disklordz/agents/profit/billing-ops/agent.md) |
| 3 | ship-velocity | Ship Velocity | [agent.md](../disklordz/agents/profit/ship-velocity/agent.md) |
| 4 | async-generation | Async Generation | [agent.md](../disklordz/agents/profit/async-generation/agent.md) |
| 5 | prompt-coach | Prompt Coach | [agent.md](../disklordz/agents/profit/prompt-coach/agent.md) |
| 6 | product-factory | Product Factory | [agent.md](../disklordz/agents/profit/product-factory/agent.md) |
| 7 | spec-validator | Spec Validator | [agent.md](../disklordz/agents/profit/spec-validator/agent.md) |
| 8 | lane-workflow | Lane Workflow | [agent.md](../disklordz/agents/profit/lane-workflow/agent.md) |
| 9 | ops-schema | Ops Schema | [agent.md](../disklordz/agents/profit/ops-schema/agent.md) |
| 10 | marketing-glue | Marketing Glue | [agent.md](../disklordz/agents/profit/marketing-glue/agent.md) |
| 11 | support-macro | Support Macro | [agent.md](../disklordz/agents/profit/support-macro/agent.md) |
| 12 | desktop-ops | Desktop Ops | [agent.md](../disklordz/agents/profit/desktop-ops/agent.md) |
| 13 | factory-batch-gpu | Factory Batch GPU | [agent.md](../disklordz/agents/profit/factory-batch-gpu/agent.md) |
| 14 | engine-swap | Engine Swap | [agent.md](../disklordz/agents/profit/engine-swap/agent.md) |
| 15 | integration-health | Integration Health | [agent.md](../disklordz/agents/profit/integration-health/agent.md) |

**Phase 2 (15 more ideas):** [DISKLORDZ_PROFIT_AGENTS_EXTENDED.md](DISKLORDZ_PROFIT_AGENTS_EXTENDED.md)

## Automation (infra)

```bash
bash disklordz/integrations/scripts/activate-integrations.sh --all
```

Blueprint SOP: Claude Code architecture (async generator loop, hooks, memory files, MCP boundaries) — encoded in each agent's `loop.md` and `hooks.md`.
