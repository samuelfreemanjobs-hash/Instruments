# Agents — Instruments monorepo

Read **`/ARCHITECTURE.md`** first, then the product `ARCHITECTURE.md` for the area you edit.

## Hermes dev team (default)

We develop and code projects with the **Hermes seat team** (Cursor agents + skills + evidence).

| Start here | Path |
|------------|------|
| Quickstart | [docs/HERMES_QUICKSTART.md](docs/HERMES_QUICKSTART.md) |
| Framework | [docs/HERMES_AGENT_FRAMEWORK.md](docs/HERMES_AGENT_FRAMEWORK.md) |
| Skills | [.cursor/hermes/SKILLS_REGISTRY.md](.cursor/hermes/SKILLS_REGISTRY.md) |
| Grok WOs | [docs/GROK_CLOSED_LOOP_ENGINE.md](docs/GROK_CLOSED_LOOP_ENGINE.md) |

**Seats (16):** [docs/HERMES_SEATS.md](docs/HERMES_SEATS.md) — lead, architect, dsp, gui, web, qa, research, devops, handoff, ops, **sop**, gtm, presets, support, security, data

**Operating procedures:** [docs/HERMES_SOP_OPERATIONS.md](docs/HERMES_SOP_OPERATIONS.md) · `python3 disklordz/hermes/scripts/hermes_tool.py sop audit`

```bash
python3 disklordz/hermes/scripts/hermes_tool.py --help
python3 disklordz/research/scripts/hyperresearch.py --topic "your question" --product junova --write
```

## Standards

- [docs/CURSOR_AGENT_PLAYBOOK.md](docs/CURSOR_AGENT_PLAYBOOK.md) — Agent Mode, Cloud, structured prompts  
- [docs/AGENTIC_PROJECT_STANDARDS.md](docs/AGENTIC_PROJECT_STANDARDS.md) — rules, PR policy, definition of done  
- `.cursor/rules/*.mdc` — always-on architecture, security, **hermes-default**  

## Disklordz SaaS (web) — `hermes-web`

```bash
cd disklordz/website && npm ci && npm run build && npm test # if tests exist
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

## JUCE plugins — `hermes-architect` / `hermes-dsp` / `hermes-gui` / `hermes-qa`

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j
python3 vst-testing-ops/run_business.py --profile ci       # full plugin QA (matches build.yml)
python3 vst-testing-ops/run_business.py --profile dsp-only # DSP/golden only (faster)
python3 vst-testing-ops/test_runner.py                     # single-VST pluginval
streamlit run vst-testing-ops/app.py                       # operations dashboard
```

**Junova-X:**

```bash
cmake --build build -j --target JunovaX_Standalone JunovaX_VST3 JunovaX_CLAP
bash Junova-X/scripts/finish_line.sh --mode full
python3 vst-testing-ops/run_business.py --profile junova-ship
```

Finish gates: [Junova-X/docs/FINISH_LINE.md](Junova-X/docs/FINISH_LINE.md) · monorepo [docs/PRODUCT_FINISH_PLAYBOOK.md](docs/PRODUCT_FINISH_PLAYBOOK.md)

**After editing plugin C++ (`Source/`, `Wave909/`, `Junova-X/`, etc.):** run `run_business.py --profile ci` before pushing; on failure read `vst-testing-ops/error_log.txt` and fix until green. Intentional DSP output changes: `tests/golden/refresh_golden.sh` then commit updated WAVs.

- [docs/REPO_AUTOMATION.md](docs/REPO_AUTOMATION.md) — branch protection, Slack CI, golden WAV policy

## Git

- Do not force-push or deploy production unless the user asks.  
- Cloud feature branches: `cursor/<description>-<suffix>` when required by environment.  
