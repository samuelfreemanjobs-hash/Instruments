# Agent memory model

Four tiers. **Company** and **technical** files are git-tracked; **product** per SKU; **session** in SQLite.

| Tier | Storage | Loaded when |
|------|---------|-------------|
| Company | [memory/company.json](../memory/company.json) | Every Cursor export, every new project |
| Product | [memory/products/<slug>.json](../memory/products/) | Project with matching `plugin_name` or portfolio id |
| Technical | [memory/technical/*.json](../memory/technical/) | DSP/build tasks; grep by tag |
| Session | SQLite `sessions` table | Active orchestrator run |

## Dependency graph

[memory/dependency-graph.json](../memory/dependency-graph.json) maps shared module ids → consumer plugins. When `vst/factory-lib/` changes, run `factory` profile for each consumer.

## API

- `vst_factory.memory.load_company_memory()`
- `vst_factory.memory.load_product_memory(slug)`
- Injected into [templates/prompts/cursor-impl.md](../templates/prompts/cursor-impl.md) as `{{company_memory}}` / `{{product_memory}}`
