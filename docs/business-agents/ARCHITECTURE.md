# Business agents — architecture

## Purpose

Three **company-wide** LLM patterns (not product code):

| Agent | Pattern | Output | Human gate |
|-------|---------|--------|------------|
| **SKU research** | #5 / #9 | `business-agents/sku-briefs/<id>/brief.json` | Business Planner approves `product_id` |
| **Content** | #2 | `business-agents/content-drafts/*.json` | Owner **publishes** |
| **Ship evaluator** | #8 | `business-agents/ship-evaluations/*.json` | Required before **RELEASE READY** / customer-facing merge |

Prompts: [prompts/](prompts/) · Schemas: [schemas/](schemas/) · Workflows: [WORKFLOWS.md](WORKFLOWS.md)

## Build & run

```bash
# Scaffold new SKU research folder
python3 scripts/business-agents/business_agent.py sku init \
  --product-id DL-PLUGIN-EXAMPLE --name "Example Synth"

# Print prompt bundle (paste into Cursor / Claude Code)
python3 scripts/business-agents/business_agent.py sku prompt \
  --intake business-agents/sku-briefs/DL-PLUGIN-EXAMPLE/intake.yaml

# Validate Planner JSON (after model saves brief.json)
python3 scripts/business-agents/business_agent.py sku validate \
  --file business-agents/sku-briefs/DL-PLUGIN-EXAMPLE/brief.json

# Optional: automated LLM (ANTHROPIC_API_KEY or OPENAI_API_KEY)
python3 scripts/business-agents/business_agent.py sku prompt --intake .../intake.yaml --run
```

Same CLI subcommands: `content prompt|validate`, `evaluate prompt|validate`, `check-pr`.

## Data flow

```text
Idea / Airtable WO
  → sku init + intake.yaml
  → sku prompt → brief.json (Planner decision)
  → GitHub issue + agent-registry row
  → Factory implements
  → content prompt → draft JSON (Marketing)
  → evaluate prompt on PRODUCT_SPEC / landing / listing
  → ACCEPT + human ship gate → merge / deploy
```

## Threading / realtime

N/A — offline CLI and CI; no audio thread.

## Key modules

| Path | Role |
|------|------|
| `scripts/business-agents/business_agent.py` | CLI |
| `scripts/business-agents/lib/schema_validate.py` | Stdlib JSON gate |
| `scripts/business-agents/lib/llm_optional.py` | Optional API completion |
| `integrations/automation/business-agents.yaml` | Schedule hints (human-in-loop) |
| `.github/workflows/business-agents.yml` | PR validation + manual intake |

## Extension points

- Wire Airtable WO create from `brief.json` (`planner_recommendation.proceed`).
- n8n webhook calling `business_agent.py … --run` (secrets in env only).
- Add `claudeStrategyAgents` rows in [agent-registry.json](../agent-registry.json).

## Related

- [AGENTIC_OPERATING_MODEL.md](../AGENTIC_OPERATING_MODEL.md)
- [PLUGIN_FACTORY_OS.md](../PLUGIN_FACTORY_OS.md) · [GATES.md](../plugin-factory/GATES.md)
