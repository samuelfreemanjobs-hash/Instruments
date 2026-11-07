# AI coding agent stack (Instruments monorepo)

Canonical map of the **modern six-layer agent stack** to what is implemented in this repository, and how to run it **without** Cursor IDE agent mode.

## Quick start (recommended)

| You are… | Use |
|----------|-----|
| At your desk, terminal | **Claude Code** + plain editor |
| On your phone | **Next.js PWA** → Grok Bot ([GROK_PWA_INTEGRATION.md](GROK_PWA_INTEGRATION.md)) |
| Multi-file / CI harness | **Grok Bot** or **Cursor Cloud Agent** (via Grok + Composio MCP) |
| Local VST harness smoke | Optional `tools/coding-agent-stack/` on laptop only |

Deploy the phone UI on **Vercel** (`disklordz/website`, route `/remote-agent`). **Do not** host Python on Render unless you want the optional dev harness.

---

## Layer 1 — Agents & interfaces

| Tool | Role in this repo | Path / command |
|------|-------------------|----------------|
| **Claude Code** | Local foreground CLI (git, single file) | Documented in [AGENTS.md](../AGENTS.md) |
| **Grok Bot / Grok Build** | Cloud VM — git, browser, tests, PRs | `GROK_INFERENCE_HOOK_URL` → [inference-hook.ts](../disklordz/website/src/lib/grok/inference-hook.ts) |
| **Cursor Cloud Agent** | Delegated multi-file edits | Grok → MCP/Composio; or Cloud from Cursor dashboard |
| **Phone PWA** | Remote controller | `/remote-agent`, `POST /api/agent` |
| **GitHub Copilot** | Optional inline IDE | Not wired in repo |
| **Optional Python PWA** | Dev / dry-run only | `uvicorn coding_agent_stack.main:app` — [CODING_AGENT_STACK.md](CODING_AGENT_STACK.md) |

## Layer 2 — Models

Configure on **Grok** / **Claude** / optional local stack:

- `OPENAI_API_KEY`, `OPENAI_BASE_URL` (Groq, OpenRouter) — `tools/coding-agent-stack/.env`
- Website polish: `OPENAI_*` in `disklordz/website/.env.example`

## Layer 3 — Orchestration

| Pattern | Implementation |
|---------|----------------|
| **Production (Grok)** | Grok’s own loop on cloud VM — PWA sends one HTTP task |
| **PWA team (optional)** | Architect → Sandbox Coder → QA — `coding_agent_stack/team_graph.py` |
| **LangGraph** | Optional extra `pip install 'coding-agent-stack[llm]'` |
| **Disklordz fleet** | 31 profit agents — [DISKLORDZ_AGENTS.md](../DISKLORDZ_AGENTS.md) |

## Layer 4 — Context & memory

| Source | Use |
|--------|-----|
| **AGENTS.md** | Shared by Claude Code, Grok Build, ACP payloads |
| **ARCHITECTURE.md** + product docs | First read for any agent task |
| [VST_AI_PLUGIN_MAP.md](VST_AI_PLUGIN_MAP.md) | Token-efficient JUCE file map |
| **disklordz RAG** | SaaS prompts — `disklordz/rag/` |
| **Supabase pgvector** | Website `/api/rag/suggest` |

## Layer 5 — Tools, sandbox, MCP

| Capability | Production | Optional local |
|------------|------------|----------------|
| Git / GitHub | Grok cloud + Composio MCP | — |
| Compile / test | Grok VM | `scripts/vst/autopilot-compile.sh`, `vst-testing-ops/` |
| Sandbox | **Grok persistent computer** | E2B or `.sandbox/` in coding-agent-stack |
| MCP | Composio on Grok; Cursor MCP in desktop | `disklordz/integrations/mcp/` |
| REST tools catalog | — | `GET /tools` on local FastAPI |

## Layer 6 — Harness & observability

| Harness | Command |
|---------|---------|
| **JUCE / monorepo CI** | `python3 vst-testing-ops/run_business.py --profile ci` |
| **Fast compile** | `./scripts/vst/autopilot-compile.sh` |
| **Agent stack self-test** | `pytest` in `tools/coding-agent-stack/` |
| **Tracing** | `LANGFUSE_*` (optional) |

Mobile presets (QA / bugs / review): [presets.ts](../disklordz/website/src/lib/grok/presets.ts).

---

## Architecture diagram (your setup)

```text
                    ┌── Claude Code (local CLI)
                    │
[Phone] ──► Vercel PWA ──webhook──► Grok Bot (cloud VM)
                    │                      │
                    │                      ├── GitHub / tests / draft PR
                    │                      └── MCP (Composio) ──► Cursor Cloud Agent
                    │
                    └── (optional laptop) coding-agent-stack + vst-testing-ops
```

## Related docs

- [GROK_PWA_INTEGRATION.md](GROK_PWA_INTEGRATION.md) — inference hook env vars, Composio  
- [CODING_AGENT_STACK.md](CODING_AGENT_STACK.md) — optional Python harness  
- [CURSOR_VST_ENGINEER.md](CURSOR_VST_ENGINEER.md) — JUCE realtime rules  
- [CURSOR_AGENT_PLAYBOOK.md](CURSOR_AGENT_PLAYBOOK.md) — Cloud Agent prompts  
