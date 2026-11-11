# Agents — Instruments monorepo

Read **`/ARCHITECTURE.md`** first, then the product `ARCHITECTURE.md` for the area you edit.

## Agent memory (every agent, every task)

Lessons from previous agents live in **[disklordz/agents/memory/MEMORY.md](disklordz/agents/memory/MEMORY.md)** ([SOP-019](docs/disklordz/sops/SOP-019-agent-memory.md)).

```bash
python3 scripts/agent_memory.py recall --agent <id> <task keywords>   # at start (vst-factory-director | shared | profit agent id)
python3 scripts/agent_memory.py add --agent <id> --kind learning ...  # at end, for each new verified lesson
python3 scripts/agent_memory.py check                                 # must pass before you report done
```

## Disklordz agent fleet (repo-wide)

All profit agents are **company-wide** in this monorepo — not only under `disklordz/agents/`.

| Start here | Role |
|------------|------|
| **[DISKLORDZ_AGENTS.md](DISKLORDZ_AGENTS.md)** | Root fleet index + active CI |
| **[disklordz/agents/workflows/PM_ADD.md](disklordz/agents/workflows/PM_ADD.md)** | PM ADD roster (announcements) |
| **[`.github/agents/fleet.json`](.github/agents/fleet.json)** | Machine registry |
| **Orchestration** | `workflow-automation` (scaffold workflows) · **`pm-agent`** (governance + execute CI) |

```bash
./scripts/run-agent-fleet-mvp.sh          # MVP motion → docs/fleet-activity-latest.md
./scripts/run-agent-fleet-now.sh          # sync + execute (+ MVP motion lite tail)
./scripts/complete-roadmap-automation.sh  # integrations promote + 007 smoke + stub blockers
./scripts/sync-disklordz-agent-fleet.sh   # regenerate fleet docs after _specs.json edits
```

[MVP agent motion](docs/DISKLORDZ_MVP_AGENT_MOTION.md) — scheduled CI **08:00 & 16:00 UTC** weekdays.

```bash
./scripts/unlock-agent-fleet-prod.sh https://your-production-url   # Tier C CI unlock
```

## Standards

- [docs/OPERATOR_AUTOPILOT.md](docs/OPERATOR_AUTOPILOT.md) — “I don’t know how” → GitHub **Agent run** issue → Cursor Cloud
- [docs/CURSOR_AGENT_PLAYBOOK.md](docs/CURSOR_AGENT_PLAYBOOK.md) — Agent Mode, Cloud, structured prompts  
- [docs/AGENTIC_PROJECT_STANDARDS.md](docs/AGENTIC_PROJECT_STANDARDS.md) — rules, PR policy, definition of done  
- `.cursor/rules/*.mdc` — always-on architecture and security  
- **Windows (My Machines worker):** [`.cursor/rules/windows-worker.mdc`](.cursor/rules/windows-worker.mdc) — the Linux build commands below do not work there  

## AI team (Cursor / Antigravity / others)

Root **`.cursorrules`** — delegation to Antigravity, Factory liaison, business copilot. Git bridge: [disklordz/antigravity/ARCHITECTURE.md](disklordz/antigravity/ARCHITECTURE.md).

**Antigravity** runs parallel prep on Windows (inbox/outbox): [active mode](docs/ANTIGRAVITY_ACTIVE_MODE.md) · **product factory** [ANTIGRAVITY_PRODUCT_FACTORY.md](docs/ANTIGRAVITY_PRODUCT_FACTORY.md) (`sync-product-prep.py`) · **Claude Code + DPF** [ANTIGRAVITY_CLAUDE_CODE_COPILOT.md](docs/ANTIGRAVITY_CLAUDE_CODE_COPILOT.md) (`request-claude-support.sh`). **24/7 allowlisted coder:** [AUTONOMOUS_CODING_AGENT.md](docs/AUTONOMOUS_CODING_AGENT.md) · agent `autonomous-coding-agent` · `scripts/autonomous-coder/tick.py`.

## DiskLordz Audio (plugins — CHORD ENGINE)

**Business / roadmap:** agent **`audio-business-copilot`** (presents **one task** per session) · Coach PWA `http://127.0.0.1:8765/pwa/` · outreach [disklordz/audio-command/docs/OUTREACH_SETUP.md](disklordz/audio-command/docs/OUTREACH_SETUP.md)

**VST factory liaison (no C++):** agent **`plugin-factory-liaison`** · [docs/CURSOR_FACTORY_LIAISON.md](docs/CURSOR_FACTORY_LIAISON.md) · skill `disklordz-plugin-factory-liaison`

**VST engineering (implementer Cursor only):** [docs/DiskLordz_Audio_Master_Roadmap_for_Cursor.md](docs/DiskLordz_Audio_Master_Roadmap_for_Cursor.md) §18–20 · rule `.cursor/rules/disklordz-audio-engineering-lane.mdc`

Do not mix lanes: **audio-business-copilot** = sprint; **plugin-factory-liaison** = Factory prompts/CI triage; **Cursor Factory** = JUCE/C++.

**Milestone 001 (now):** Stage B on `DiskLordz-Audio/` · Windows VST3 CI [`.github/workflows/disklordz-audio-windows.yml`](.github/workflows/disklordz-audio-windows.yml) · build [`DiskLordz-Audio/BUILD.md`](DiskLordz-Audio/BUILD.md) · Coach gate **`plugin-skeleton`** (DAW proof).

## Disklordz SaaS (web)

```bash
cd disklordz/website && npm ci && npm run build && npm test  # if tests exist
```

Deploy: [disklordz/website/DEPLOY.md](disklordz/website/DEPLOY.md). Env: `NEXT_PUBLIC_SUPABASE_*`, `SUPABASE_SERVICE_ROLE_KEY` (prod kits), optional `SAAS_DAILY_GEN_LIMIT`.

**Supabase MCP (Cursor):** [disklordz/website/docs/SUPABASE_MCP.md](disklordz/website/docs/SUPABASE_MCP.md) — `.cursor/mcp.json`, set `SUPABASE_PROJECT_REF` (+ optional `SUPABASE_ACCESS_TOKEN`), then Connect in Cursor MCP settings.

**Stripe MCP (Cursor):** [disklordz/website/docs/STRIPE_MCP.md](disklordz/website/docs/STRIPE_MCP.md) — install Stripe plugin or OAuth **stripe** server; optional `STRIPE_RESTRICTED_KEY` for agents.

Roadmap: [docs/DISKLORDZ_ILLUGEN_RESEARCH.md](docs/DISKLORDZ_ILLUGEN_RESEARCH.md) (WO-SAAS-007+).

Integrations hub: [disklordz/integrations/ARCHITECTURE.md](disklordz/integrations/ARCHITECTURE.md) · `./scripts/setup-open-source-integrations.sh`

Profit agents (35): [DISKLORDZ_AGENTS.md](DISKLORDZ_AGENTS.md) · **24/7 coder:** `autonomous-coding-agent` ([docs/AUTONOMOUS_CODING_AGENT.md](docs/AUTONOMOUS_CODING_AGENT.md)) · [docs/DISKLORDZ_PROFIT_AGENTS.md](docs/DISKLORDZ_PROFIT_AGENTS.md) · `disklordz/agents/profit/<id>/agent.md`

**Roadmap & progress:** [docs/ROADMAP.md](docs/ROADMAP.md)  
**SOPs & templates:** [docs/disklordz/README.md](docs/disklordz/README.md)

META charter (all agents): [CLAUDE.md](CLAUDE.md) · sync: `./scripts/sync-meta-llm-charter.sh` · skills `/zero-pause` `/weave` `/premortem`

## RAG (prompt knowledge)

```bash
python3 disklordz/rag/scripts/chunk_corpus.py
python3 disklordz/rag/scripts/query_local.py "your query"
```

Colab: [docs/COLAB_ZERO_INSTALL_TESTING.md](docs/COLAB_ZERO_INSTALL_TESTING.md).

## SP-1200 Drumulator

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target SP1200_Standalone SP1200_VST3 SP1200MemoryTests SP1200SequencerTests SP1200ProjectFileTests
ctest --test-dir build -R SP1200 --output-on-failure
```

Handoff: [docs/SP1200_HANDOFF.md](docs/SP1200_HANDOFF.md). Spec: [docs/SP1200_STANDALONE_SPEC.md](docs/SP1200_STANDALONE_SPEC.md).

## VSTi one-shot packs (MPC)

Style brief → 100 factory one-shots from any VST3:

Windows: `.\scripts\boom.ps1 -Style "..." -Vst3 "C:\Program Files\Common Files\VST3\....vst3" -Out "$env:USERPROFILE\Exports\pack"`

Linux/macOS: `./scripts/boom.sh --style "..." --vst3 "..." --out ~/Exports/pack`

Handoff: [docs/VSTI_ONE_SHOT_WINDOWS.md](docs/VSTI_ONE_SHOT_WINDOWS.md) · [docs/VSTI_ONE_SHOT_HANDOFF.md](docs/VSTI_ONE_SHOT_HANDOFF.md)

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

## NTS-1 multi-bass (logue SDK)

```bash
./tools/mkii-automate.sh bootstrap
export LOGUE_SDK="$(pwd)/.deps/logue-sdk"
./tools/build-all-bass-mkii.sh   # NTS-1 mkII (.nts1mkiiunit)
./tools/build-all-bass.sh        # NTS-1 mkI (.ntkdigunit)
./tools/package-nts1.sh
```

Install today: [docs/NTS1_INSTALL_TODAY.md](docs/NTS1_INSTALL_TODAY.md). Catalog: [docs/nts1-multi-bass-oscillators.md](docs/nts1-multi-bass-oscillators.md). mkII porting: [docs/nts1-mkii-porting.md](docs/nts1-mkii-porting.md).

## Audio Plugin Coder (optional VST scaffold)

New plugins under `vst/`: [docs/apc-workflow.md](docs/apc-workflow.md) · `/apc-setup` … `/apc-ship` · [docs/vst-lifecycle.md](docs/vst-lifecycle.md).

## DiskLordz VST Factory (AI plugin studio)

```bash
cd disklordz/vst-factory && python3 -m venv .venv && source .venv/bin/activate
pip install -e ".[dev]" && pytest tests -q
uvicorn vst_factory.main:app --reload --port 8790
```

See [disklordz/vst-factory/ARCHITECTURE.md](disklordz/vst-factory/ARCHITECTURE.md) · [docs/DISKLORDZ_VST_FACTORY.md](docs/DISKLORDZ_VST_FACTORY.md).

QA for a scaffolded APC plugin: `python3 vst-testing-ops/run_business.py --profile factory --plugin <Name>`.

## SynthForge (preset workstation)

```bash
cd tools/synth-forge && python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt && pip install -e . && pytest tests -v
uvicorn synth_forge.main:app --reload --port 8000
```

See [tools/synth-forge/ARCHITECTURE.md](tools/synth-forge/ARCHITECTURE.md).

**Fleet agent:** `hardware-preset-designer` — read `disklordz/agents/profit/hardware-preset-designer/agent.md` or skill `disklordz-hardware-preset-designer`. CLI: `python3 disklordz/integrations/agents/hardware_preset_designer.py --help`.

## Git

- Do not force-push or deploy production unless the user asks.  
- Cloud feature branches: `cursor/<description>-<suffix>` when required by environment.  
