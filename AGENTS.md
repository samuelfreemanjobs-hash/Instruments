# Agents — Instruments monorepo

Read **`/ARCHITECTURE.md`** first, then the product `ARCHITECTURE.md` for the area you edit.

## Standards

- [docs/CURSOR_AGENT_PLAYBOOK.md](docs/CURSOR_AGENT_PLAYBOOK.md) — Agent Mode, Cloud, structured prompts 
- [docs/AGENTIC_PROJECT_STANDARDS.md](docs/AGENTIC_PROJECT_STANDARDS.md) — rules, PR policy, definition of done 
- [docs/AGENTIC_OPERATING_MODEL.md](docs/AGENTIC_OPERATING_MODEL.md) — one-page company workflow (Instruments + Disklordz)
- [CLAUDE.md](CLAUDE.md) — Claude Code entry (parallel to this file)
- [docs/CLAUDE_CODE_INTEGRATION.md](docs/CLAUDE_CODE_INTEGRATION.md) — Cursor + Claude handoffs
- [docs/agent-registry.json](docs/agent-registry.json) — which agent owns which product path
- `.cursor/rules/*.mdc` — always-on architecture and security  

## Disklordz SaaS (web)

```bash
cd disklordz/website && npm ci && npm run build && npm test  # if tests exist
```

Deploy: [disklordz/website/DEPLOY.md](disklordz/website/DEPLOY.md). Env: `NEXT_PUBLIC_SUPABASE_*`, `SUPABASE_SERVICE_ROLE_KEY` (prod kits), optional `SAAS_DAILY_GEN_LIMIT`.

**Supabase MCP (Cursor):** [disklordz/website/docs/SUPABASE_MCP.md](disklordz/website/docs/SUPABASE_MCP.md) — `.cursor/mcp.json`, set `SUPABASE_PROJECT_REF` (+ optional `SUPABASE_ACCESS_TOKEN`), then Connect in Cursor MCP settings.

**Stripe MCP (Cursor):** [disklordz/website/docs/STRIPE_MCP.md](disklordz/website/docs/STRIPE_MCP.md) — install Stripe plugin or OAuth **stripe** server; optional `STRIPE_RESTRICTED_KEY` for agents.

Roadmap: [docs/DISKLORDZ_ILLUGEN_RESEARCH.md](docs/DISKLORDZ_ILLUGEN_RESEARCH.md) (WO-SAAS-007+).

## RAG (prompt knowledge)

```bash
python3 disklordz/rag/scripts/chunk_corpus.py
python3 disklordz/rag/scripts/query_local.py "your query"
```

Colab: [docs/COLAB_ZERO_INSTALL_TESTING.md](docs/COLAB_ZERO_INSTALL_TESTING.md).

## JUCE plugin (default Cloud install)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j
```

## Git

- Do not force-push or deploy production unless the user asks. 
- Cloud feature branches: `cursor/<description>-<suffix>` when required by environment.

## Plugin Factory (JUCE products)

New or extended plugins: read [docs/PLUGIN_FACTORY_OS.md](docs/PLUGIN_FACTORY_OS.md) first.

- Manifest: `<repoPath>/plugin.manifest.json` ([schema](docs/plugin-factory/manifest.schema.json))
- Gates: `<repoPath>/Docs/GATES.md`
- Reference implementation: [Wave909/ARCHITECTURE.md](Wave909/ARCHITECTURE.md)
