# Agents — Instruments monorepo

Read **`/ARCHITECTURE.md`** first, then the product `ARCHITECTURE.md` for the area you edit.

## Standards

- [docs/CURSOR_AGENT_PLAYBOOK.md](docs/CURSOR_AGENT_PLAYBOOK.md) — Agent Mode, Cloud, structured prompts  
- [docs/AGENTIC_PROJECT_STANDARDS.md](docs/AGENTIC_PROJECT_STANDARDS.md) — rules, PR policy, definition of done  
- `.cursor/rules/*.mdc` — always-on architecture and security  

## JUCE factory elite worker (Track C, autonomous)

**Role:** `juce-factory-elite` — senior JUCE/C++20 implementer for `Source/`, `Wave909/`, `tools/`.

| Artifact | Purpose |
|----------|---------|
| [docs/JUCE_FACTORY_AGENT.md](docs/JUCE_FACTORY_AGENT.md) | Autonomy loop, definition of done, automation hooks |
| [docs/prompts/juce-factory-worker.cloud.md](docs/prompts/juce-factory-worker.cloud.md) | Copy-paste **Cursor Cloud Agent** mission |
| [.cursor/rules/juce-factory-elite.mdc](.cursor/rules/juce-factory-elite.mdc) | Realtime + CI rules (JUCE globs) |
| `./scripts/agents/print-juce-factory-prompt.sh WO-…` | Print bootstrap prompt in terminal |

Launch: [Cursor Cloud Agents](https://cursor.com/agents) → repo **Instruments** → paste prompt → environment **Disklordz / Instruments**. GitHub: issue template **JUCE factory work order** (`juce-factory` label).

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
python3 vst-testing-ops/run_business.py --profile ci       # full plugin QA (matches build.yml)
python3 vst-testing-ops/run_business.py --profile dsp-only # DSP/golden only (faster)
python3 vst-testing-ops/test_runner.py                     # single-VST pluginval
streamlit run vst-testing-ops/app.py                       # operations dashboard
```

**After editing plugin C++ (`Source/`, `Wave909/`, etc.):** run `run_business.py --profile ci` before pushing; on failure read `vst-testing-ops/error_log.txt` and fix until green. Intentional DSP output changes: `tests/golden/refresh_golden.sh` then commit updated WAVs.

- [docs/REPO_AUTOMATION.md](docs/REPO_AUTOMATION.md) — branch protection, Slack CI, golden WAV policy

## Git

- Do not force-push or deploy production unless the user asks.  
- Cloud feature branches: `cursor/<description>-<suffix>` when required by environment.  
