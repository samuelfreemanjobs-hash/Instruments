# Business agents CLI

Company-wide **SKU research**, **marketing content**, and **ship evaluation** prompts + validation.

Full docs: [docs/business-agents/ARCHITECTURE.md](../../docs/business-agents/ARCHITECTURE.md)

```bash
python3 scripts/business-agents/business_agent.py sku init --product-id DL-FOO --name "Foo"
python3 scripts/business-agents/business_agent.py check-pr
```

Optional `--run` requires `ANTHROPIC_API_KEY` or `OPENAI_API_KEY` in the environment (never commit keys).
