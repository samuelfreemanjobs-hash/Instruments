# Disklordz open-source integrations hub

## Purpose

Wire the **35 reference GitHub projects** (agents, skills, MCP, RAG, async jobs, audio engines) into this monorepo without vendoring full upstream trees. Status per repo lives in [`manifest.json`](manifest.json); live health in `GET /api/integrations/status` on the website.

## Build & run

```bash
# One-shot local setup (optional Python venv + npm deps + chunk corpus)
./scripts/setup-open-source-integrations.sh

# Production: migrations, RAG embed, Vercel env, Inngest, verify
bash disklordz/integrations/scripts/activate-integrations.sh --all
# CI: Actions → Activate Disklordz integrations

# Website (required)
cd disklordz/website && npm ci && npm run build

# Optional: n8n for Airtable/GitHub glue
docker compose -f disklordz/integrations/docker-compose.optional.yml --profile n8n up -d

# Optional: GPU engine workers (see engines/README.md)
cd disklordz/integrations/engines/audiocraft && pip install -r requirements.txt
```

## Data flow

```text
Corpus (git) → chunk_corpus.py → embed_and_upsert.py → Supabase pgvector
       → hybrid retrieve (pgvector + keyword v1) → /api/rag/suggest
       → optional Vercel AI SDK polish (OPENAI_API_KEY)

POST /api/generate → engine registry (parametric default)
       → optional DISKLORDZ_*_ENGINE_URL remote WAV workers

POST /api/generate/async → Inngest event → same batch builder → Storage
```

## Threading / realtime

- **Inngest** steps run off the HTTP request; sync `/api/generate` unchanged when async disabled.
- **Remote engines** are HTTP-only from the Next.js server (never block the audio thread in plugins).

## Key modules

| Path | Role |
|------|------|
| [`manifest.json`](manifest.json) | 35-repo index + wiring paths |
| [`mcp/disklordz-mcp-server/`](mcp/disklordz-mcp-server/) | TypeScript MCP (RAG + integration status tools) |
| [`engines/`](engines/) | AudioCraft, Stable Audio, Cog, Modal stubs |
| [`agents/`](agents/) | Optional LangGraph / CrewAI / pydantic-ai runners |
| [`docker-compose.optional.yml`](docker-compose.optional.yml) | n8n profile |
| [`../website/src/inngest/`](../website/src/inngest/) | Async generate + RAG reindex |
| [`../website/src/lib/generation/engines/`](../website/src/lib/generation/engines/) | Engine registry |
| [`.github/skills/`](../../.github/skills/) | Agent skills (anthropics/skills shape) |

## Extension points

- Set `DISKLORDZ_ENGINE=parametric|remote` and engine URLs in `disklordz/website/.env`.
- Apply pgvector migration on Supabase; run `embed_and_upsert.py` after doc changes.
- Enable Inngest with `INNGEST_EVENT_KEY` + `/api/inngest` on Vercel.

## Related docs

- [docs/DISKLORDZ_PROFIT_AGENTS.md](../../docs/DISKLORDZ_PROFIT_AGENTS.md) — 15 automatable profit agents
- [docs/DISKLORDZ_OPEN_SOURCE_REFERENCES.md](../../docs/DISKLORDZ_OPEN_SOURCE_REFERENCES.md)
- [disklordz/rag/ARCHITECTURE.md](../rag/ARCHITECTURE.md)
- [disklordz/website/ARCHITECTURE.md](../website/ARCHITECTURE.md)
