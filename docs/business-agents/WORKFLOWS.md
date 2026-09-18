# Business agent workflows

## 1. New SKU (Planner)

1. **Init:** `python3 scripts/business-agents/business_agent.py sku init --product-id <ID> --name "<Name>"`
2. Edit `business-agents/sku-briefs/<ID>/intake.yaml` (lane, question, constraints).
3. **Research:** `sku prompt --intake …/intake.yaml` → run in Cursor/Claude → save **`brief.json`** in same folder.
4. **Validate:** `sku validate --file …/brief.json`
5. If `planner_recommendation.decision` is `proceed`: open GitHub issue / Airtable WO using `suggested_wo_title`; register [agent-registry.json](../agent-registry.json).
6. Set `human_approved: true` in JSON only after you sign off.

## 2. Marketing content (draft)

1. `content prompt --type landing_hero --product WAVE-909 --topic "…" [--spec Wave909/Docs/PRODUCT_SPEC.md]`
2. Save model output JSON under `business-agents/content-drafts/`
3. `content validate --file …`
4. Owner copies approved body to store/website — **never** auto-publish from CI.

## 3. Ship / RELEASE READY (evaluator)

Before calling a plugin **RC**, SaaS launch, or merging customer-facing copy:

1. `evaluate prompt --artifact <path> --type product_spec|landing_copy|…`
2. Save **`ship-evaluations/<name>-eval.json`**
3. `evaluate validate --file …`
4. Merge only if `quality_assessment` is **ACCEPT** and `ready_for_approval` is **true**, or you document waivers in PR.

Link evaluator paths in PR description for factory runs.

## 4. CI / GitHub Actions

| Trigger | Action |
|---------|--------|
| PR touches `business-agents/**/*.json` | `business_agent.py check-pr` must pass |
| `workflow_dispatch` **SKU intake** | Runs `sku init` with inputs; opens nothing automatically — commit scaffold locally or use generated artifact |
| Label `ship-eval-waived` | Skips optional customer-facing path check (use sparingly) |

Workflow file: [`.github/workflows/business-agents.yml`](../../.github/workflows/business-agents.yml)

## 5. Scheduled automation (optional)

See [`integrations/automation/business-agents.yaml`](../../integrations/automation/business-agents.yaml). Cron jobs **remind** you to run prompts; they do not call paid APIs unless `BUSINESS_AGENT_CRON_RUN=1` and keys are set on the runner.

## Security

- No API keys in git; use repo secrets for `--run` in private Actions only.
- Treat model research as **untrusted** until validated and `human_approved`.
- Evaluator does not replace DAW smoke, pluginval, or prod deploy approval.
