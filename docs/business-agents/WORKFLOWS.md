# Business agent workflows (team — no API)

## Assign a teammate

```bash
python3 scripts/business-agents/business_agent.py team \
  --role sku-research \
  --request "Research DL-PLUGIN-FOO using intake at business-agents/sku-briefs/DL-PLUGIN-FOO/intake.yaml"
```

Paste the output into a **Cursor Cloud** task, or run Claude Code with the matching `.claude/agents/business-*/AGENT.md`.

Roles: `sku-research` | `content` | `ship-eval`

## 1. New SKU (Planner)

1. `sku init` → edit `intake.yaml`
2. **Cloud/Claude** as **business-sku-research** → `brief.json`
3. `sku validate`
4. Planner approves → factory WO + `agent-registry.json`

## 2. Marketing content

1. **business-content** teammate with topic + optional `--spec` path in task
2. `content validate`
3. Owner publishes (never CI)

## 3. Ship evaluator

1. **business-ship-eval** on customer-facing file
2. `evaluate validate`
3. Attach eval JSON to PR; human gates still apply

## CI

[`.github/workflows/business-agents.yml`](../../.github/workflows/business-agents.yml) — JSON validation + SKU scaffold artifact only.

## Security

- No `ANTHROPIC_API_KEY` / `OPENAI_API_KEY` in business-agent scripts or workflows.
- Teammates follow [security baseline](../../.cursor/rules/security-baseline.mdc).
