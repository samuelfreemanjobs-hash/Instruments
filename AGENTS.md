# Agents — Instruments monorepo

Read **`/ARCHITECTURE.md`** first, then the product `ARCHITECTURE.md` for the area you edit.

## Standards

- [docs/CURSOR_AGENT_PLAYBOOK.md](docs/CURSOR_AGENT_PLAYBOOK.md) — Agent Mode, Cloud, structured prompts  
- [docs/AGENTIC_PROJECT_STANDARDS.md](docs/AGENTIC_PROJECT_STANDARDS.md) — rules, PR policy, definition of done  
- `.cursor/rules/*.mdc` — always-on architecture and security  

## logue SDK mkII custom oscillator agent (NTS-1 mkII standard)

**Primary lane:** design and implement **SDK v2** user oscillators for **NTS-1 mkII** (`.nts1mkiiunit`).

| Doc | Purpose |
|-----|---------|
| [docs/logue-mkii-oscillator-agent.md](docs/logue-mkii-oscillator-agent.md) | Agent workflow, deliverables, evaluation |
| [docs/logue-mkii-workflow.md](docs/logue-mkii-workflow.md) | `/logue-mkii-*` slash commands & phases |
| [.cursor/agents/logue-mkii-oscillator/SYSTEM.md](.cursor/agents/logue-mkii-oscillator/SYSTEM.md) | Master system prompt |
| [docs/nts1-mkii-porting.md](docs/nts1-mkii-porting.md) | Port v1.1 → mkII, build, load |
| [docs/nts1-multi-bass-oscillators.md](docs/nts1-multi-bass-oscillators.md) | Catalog + presets |
| [docs/logue-mkii-golden-unit.md](docs/logue-mkii-golden-unit.md) | Golden reference unit for CI/agents |
| [docs/logue-oscillator-ideas-backlog.md](docs/logue-oscillator-ideas-backlog.md) | Idea list (keep & revisit) |
| [docs/logue-agent-pm-automation.md](docs/logue-agent-pm-automation.md) | **Korg Logue Agent** weekly PM + team integration |

```bash
chmod +x tools/mkii-automate.sh tools/osc-eval-mkii.sh
./tools/mkii-automate.sh bootstrap          # LOGUE_SDK + ARM gcc (once)
./tools/mkii-automate.sh scaffold           # refresh mkII trees
./tools/osc-eval-mkii.sh tr808_kick_phonk   # build + smoke checks (golden)
./tools/build-mkii.sh <slug>                # single unit when bootstrapped
```

**New mkII unit:** `./tools/new-mkii-oscillator.sh <slug> ["Name"]` — see [docs/logue-mkii-boilerplate.md](docs/logue-mkii-boilerplate.md).

Optional **v1.1** builds (legacy): `./tools/build.sh oscillators/<name> minilogue-xd` — new collection is **mkII-only** per [docs/logue-oscillator-collection-roadmap.md](docs/logue-oscillator-collection-roadmap.md).

Legacy **Claude oscillator bundle** (v1 fiction API): quarantined under [docs/archive/quarantined-claude-osc-bundle/](docs/archive/quarantined-claude-osc-bundle/) — do not use for codegen.

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

**APC / VST lane** (when editing `vst/`): [docs/apc-workflow.md](docs/apc-workflow.md) · `.agents/workflows/apc-*.md`

## Git

- Do not force-push or deploy production unless the user asks.  
- Cloud feature branches: `cursor/<description>-<suffix>` when required by environment.
