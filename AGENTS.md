# Agents — Instruments monorepo

Read **`/ARCHITECTURE.md`** first, then the product `ARCHITECTURE.md` for the area you edit.

## Disklordz agent fleet (repo-wide)

All profit agents are **company-wide** in this monorepo — not only under `disklordz/agents/`.

| Start here | Role |
|------------|------|
| **[DISKLORDZ_AGENTS.md](DISKLORDZ_AGENTS.md)** | Root fleet index + active CI |
| **[disklordz/agents/workflows/PM_ADD.md](disklordz/agents/workflows/PM_ADD.md)** | PM ADD roster (announcements) |
| **[`.github/agents/fleet.json`](.github/agents/fleet.json)** | Machine registry |
| **Orchestration** | `workflow-automation` (scaffold workflows) · **`pm-agent`** (governance + execute CI) |

```bash
./scripts/run-agent-fleet-now.sh          # sync + execute all local agent roles now
./scripts/complete-roadmap-automation.sh  # integrations promote + 007 smoke + stub blockers
./scripts/sync-disklordz-agent-fleet.sh   # regenerate fleet docs after _specs.json edits
```

## Standards

- [docs/CURSOR_AGENT_PLAYBOOK.md](docs/CURSOR_AGENT_PLAYBOOK.md) — Agent Mode, Cloud, structured prompts  
- [docs/AGENTIC_PROJECT_STANDARDS.md](docs/AGENTIC_PROJECT_STANDARDS.md) — rules, PR policy, definition of done  
- `.cursor/rules/*.mdc` — always-on architecture and security  

## Disklordz SaaS (web)

```bash
cd disklordz/website && npm ci && npm run build && npm test  # if tests exist
```

Deploy: [disklordz/website/DEPLOY.md](disklordz/website/DEPLOY.md). Env: `NEXT_PUBLIC_SUPABASE_*`, `SUPABASE_SERVICE_ROLE_KEY` (prod kits), optional `SAAS_DAILY_GEN_LIMIT`.

**Supabase MCP (Cursor):** [disklordz/website/docs/SUPABASE_MCP.md](disklordz/website/docs/SUPABASE_MCP.md) — `.cursor/mcp.json`, set `SUPABASE_PROJECT_REF` (+ optional `SUPABASE_ACCESS_TOKEN`), then Connect in Cursor MCP settings.

**Stripe MCP (Cursor):** [disklordz/website/docs/STRIPE_MCP.md](disklordz/website/docs/STRIPE_MCP.md) — install Stripe plugin or OAuth **stripe** server; optional `STRIPE_RESTRICTED_KEY` for agents.

Roadmap: [docs/DISKLORDZ_ILLUGEN_RESEARCH.md](docs/DISKLORDZ_ILLUGEN_RESEARCH.md) (WO-SAAS-007+).

Integrations hub: [disklordz/integrations/ARCHITECTURE.md](disklordz/integrations/ARCHITECTURE.md) · `./scripts/setup-open-source-integrations.sh`

Profit agents (31): [DISKLORDZ_AGENTS.md](DISKLORDZ_AGENTS.md) · [docs/DISKLORDZ_PROFIT_AGENTS.md](docs/DISKLORDZ_PROFIT_AGENTS.md) · `disklordz/agents/profit/<id>/agent.md`

**Roadmap & progress:** [docs/ROADMAP.md](docs/ROADMAP.md)  
**SOPs & templates:** [docs/disklordz/README.md](docs/disklordz/README.md)

META charter (all agents): [CLAUDE.md](CLAUDE.md) · sync: `./scripts/sync-meta-llm-charter.sh` · skills `/zero-pause` `/weave` `/premortem`

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
python3 vst-testing-ops/run_business.py --profile ci       # full plugin QA (matches build.yml)
python3 vst-testing-ops/run_business.py --profile dsp-only # DSP/golden only (faster)
python3 vst-testing-ops/test_runner.py                     # single-VST pluginval
streamlit run vst-testing-ops/app.py                       # operations dashboard
```

**After editing plugin C++ (`Source/`, `Wave9090/`, etc.):** run `run_business.py --profile ci` before pushing; on failure read `vst-testing-ops/error_log.txt` and fix until green. Intentional DSP output changes: `tests/golden/refresh_golden.sh` then commit updated WAVs.

- [docs/REPO_AUTOMATION.md](docs/REPO_AUTOMATION.md) — branch protection, Slack CI, golden WAV policy

## Git

- Do not force-push or deploy production unless the user asks.  
- Cloud feature branches: `cursor/<description>-<suffix>` when required by environment.  
