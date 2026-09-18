# Disklordz — monorepo package index

Parent index: [/ARCHITECTURE.md](../ARCHITECTURE.md) · **Integration gaps:** [docs/DISKLORDZ_INTEGRATION_AUDIT.md](../docs/DISKLORDZ_INTEGRATION_AUDIT.md)

## Subsystems

| Path | Purpose | ARCHITECTURE |
|------|---------|--------------|
| `website/` | SaaS + `/launch` funnel + APIs | [website/ARCHITECTURE.md](website/ARCHITECTURE.md) |
| `sound-factory/` | Offline/batch WAV (parametric v1) | [sound-factory/README.md](sound-factory/README.md) |
| `factory/` | Agent pipeline after generation | [factory/WORKFLOW.md](factory/WORKFLOW.md) |
| `exec/` | SAGE Chief of Staff + Slack EA | [exec/SLACK_EXECUTIVE_ASSISTANT.md](exec/SLACK_EXECUTIVE_ASSISTANT.md) |
| `marketing/` | VARA, HELM, campaigns, GTM | [marketing/ARCHITECTURE.md](marketing/ARCHITECTURE.md) |
| `ar/` | CROW, artist lanes, QC | [ar/ARCHITECTURE.md](ar/ARCHITECTURE.md) |
| `automation/` | FORGE, Airtable handoff, PM scheduling | [automation/README.md](automation/README.md) |
| `rag/` | Corpus + local query | [rag/ARCHITECTURE.md](rag/ARCHITECTURE.md) |
| `antigravity/` | HISE handoff inbox/outbox | [antigravity/ARCHITECTURE.md](antigravity/ARCHITECTURE.md) |
| `launch-assets/` | Shippable ZIPs (tripwire, stems) | [launch-assets/README.md](launch-assets/README.md) |
| `airtable/` | Seed JSON | — |
| `colab/` | Notebook smoke | — |

## Factory data flow (actual today)

```text
Prompt + preset
    → website/src/lib/generation/*  (primary)
    → sound-factory/scripts/generate_kit.py  (offline batch; stub-backed)
    → NOT YET: launch-assets/*.zip without a WO + manual QC
```

## Leadership vs implementation

| Layer | Who |
|-------|-----|
| **Leadership / GTM** | You, PM, VARA, HELM, CROW (charters + decisions) |
| **Automation** | FORGE + GitHub Actions + Vercel cron |
| **Implementation** | **Cursor Cloud Agent** ([CURSOR_IMPLEMENTATION_LEAD.md](../docs/CURSOR_IMPLEMENTATION_LEAD.md)) |

Do not expect the coding agent to be COO. Install leadership in **agent charters + PM**, not bigger single prompts.
