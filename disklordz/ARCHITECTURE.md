# Disklordz — monorepo package index

## Purpose

**Disklordz** is the drum SaaS, sound factory, RAG, integrations hub, and AI agent fleet inside the **Instruments** repository. This file indexes every Disklordz product for agents and PM.

## Products

| Area | ARCHITECTURE | Build / run |
|------|--------------|-------------|
| **Web SaaS** | [website/ARCHITECTURE.md](website/ARCHITECTURE.md) | `cd website && npm run build` |
| **Sound factory** | [sound-factory/ARCHITECTURE.md](sound-factory/ARCHITECTURE.md) | `python3 sound-factory/scripts/generate_kit.py` |
| **MPC TRAP-FORGE** | [trap-forge/ARCHITECTURE.md](trap-forge/ARCHITECTURE.md) | `cd trap-forge && python3 serve.py` |
| **RAG** | [rag/ARCHITECTURE.md](rag/ARCHITECTURE.md) | `python3 rag/scripts/chunk_corpus.py` |
| **Integrations (35 OSS)** | [integrations/ARCHITECTURE.md](integrations/ARCHITECTURE.md) | `./scripts/setup-open-source-integrations.sh` |
| **Agent fleet (31)** | [agents/ARCHITECTURE.md](agents/ARCHITECTURE.md) · [../DISKLORDZ_AGENTS.md](../DISKLORDZ_AGENTS.md) | `./scripts/run-agent-fleet-now.sh` |
| **DAW inbox** | [daw-inbox/ARCHITECTURE.md](daw-inbox/ARCHITECTURE.md) | `cd daw-inbox && npm start` |
| **Antigravity bridge** | [antigravity/ARCHITECTURE.md](antigravity/ARCHITECTURE.md) | `./scripts/antigravity-bridge/antigravity-bridge.sh` |
| **META charter** | [agents/charter/ARCHITECTURE.md](agents/charter/ARCHITECTURE.md) | `./scripts/sync-meta-llm-charter.sh` |

## Data flow (high level)

```text
Prompt / WO → website API → sound-factory WAVs → ZIP / Supabase kits
           → RAG corpus → /api/rag/suggest
           → profit agents (CI + runtime) → integrations hub
```

## Related docs

- [docs/disklordz/README.md](../docs/disklordz/README.md) — SOPs & Cursor templates  
- [docs/ROADMAP.md](../docs/ROADMAP.md) — progress dashboard  
- [docs/DISKLORDZ_SAAS_V0.md](../docs/DISKLORDZ_SAAS_V0.md) · [docs/DISKLORDZ_ILLUGEN_RESEARCH.md](../docs/DISKLORDZ_ILLUGEN_RESEARCH.md)
