# Agents — Instruments monorepo

Read **`/ARCHITECTURE.md`** first, then the product `ARCHITECTURE.md` for the area you edit.

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

**GitHub MCP (Cursor):** [docs/GITHUB_MCP.md](docs/GITHUB_MCP.md) — `.cursor/mcp.json` **github** server; set `GITHUB_TOKEN` (PAT) or Connect in MCP settings; Cloud secret + `api.githubcopilot.com` egress.

Roadmap: [docs/DISKLORDZ_ILLUGEN_RESEARCH.md](docs/DISKLORDZ_ILLUGEN_RESEARCH.md) (WO-SAAS-007+).

## Instrument & Sound Factory agent

Enterprise design: [docs/INSTRUMENT_SOUND_FACTORY_AGENT.md](docs/INSTRUMENT_SOUND_FACTORY_AGENT.md). Entry skill: `.cursor/skills/instrument-orchestrator/SKILL.md`.

```bash
python3 disklordz/sound-factory/scripts/seed_jz400_catalog.py
python3 disklordz/sound-factory/scripts/run_product_batch.py \
  --catalog disklordz/sound-factory/catalog/jz400_catalog.json \
  --out disklordz/sound-factory/out --limit 50 --offset 0
./disklordz/sound-factory/tests/run_tests.sh
python3 disklordz/rag/scripts/query_local.py "memphis phonk lane"
```

Products: [docs/products/JZ400.md](docs/products/JZ400.md) · [docs/products/REV2_TRAP128.md](docs/products/REV2_TRAP128.md) · workflow [docs/FACTORY_AGENTIC_WORKFLOW.md](docs/FACTORY_AGENTIC_WORKFLOW.md) · improvements [docs/FACTORY_IMPROVEMENT_ROADMAP.md](docs/FACTORY_IMPROVEMENT_ROADMAP.md)

Rev2Trap generator:

```bash
cmake --build build -j --target Rev2TrapOfflineRender Rev2TrapTests
python3 disklordz/sound-factory/scripts/run_product_batch.py \
  --catalog disklordz/sound-factory/catalog/rev2_trap128_catalog.json \
  --out disklordz/sound-factory/out --engine rev2trap --limit 4
```

**Runtimes:** Cloud (JUCE + batch), local VS Code (**MPCTK** for `.xpj`), Antigravity (HISE + optional MPCTK). Same skills everywhere.

**Start runs:** you manually **or** PM Agent / Airtable WO → job JSON → Cloud Agent (skill `pm-airtable-factory-trigger`).

**MPC Keygroup:** your **Cursor MPC-agent** + [MPC-Sample-Toolkit](https://github.com/samuelfreemanjobs-hash/MPC-Sample-Toolkit) on local/Antigravity (skill `cursor-mpc-agent`). Optional: `disklordz/sound-factory/scripts/mpctk_handoff.sh <inst-dir>`.

**DawDreamer VST render:** `disklordz/sound-factory/scripts/DAWDreamer_renderer.py` (+ `requirements-dawdreamer.txt`); batch `--engine dawdreamer --vst-path …`. Full MPC-agent copy: [Origin handoff](https://origin.cursor.com/git/samuelfreemanjobs/tmp-1e342cd8506744c8.git) → import into `disklordz/mpc-agent/` ([docs/MPC_AGENT_ORIGIN_IMPORT.md](docs/MPC_AGENT_ORIGIN_IMPORT.md)).

**MPC-agent path (after import):** `disklordz/mpc-agent/` + skill `cursor-mpc-agent`.

**References:** YouTube analysis segments allowed for timbre (gitignored cache; skill `sound-design-reference-match`).

**Cloud:** heavy JUCE image only.

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

## Cloud network (Origin / MPC handoff)

- **Origin git:** `.cursor/environment.json` includes `egressAllowlist: ["origin.cursor.com"]` and `repositoryDependencies` for the MPC-agent tmp export. Confirm **Save** on the [Instruments environment](https://cursor.com/dashboard/cloud-agents/environments/e/85d21659-b124-11f1-a3d8-362438fd9788), then re-run the agent to clone the handoff.

## Git

- Do not force-push or deploy production unless the user asks. 
- Cloud feature branches: `cursor/<description>-<suffix>` when required by environment.
