# Coding Agent Stack — custom ReAct + harness + PWA

Remote **Headless Architect & task worker** for this monorepo when you are not in Cursor IDE or Claude Code. **Grok Bot** (your existing Grokbot + ACP/webhook) is the preferred **outer-loop** sandbox; Claude Code stays on the laptop. The PWA defaults to `POST /grok/send` when configured.

## Purpose

| Layer | Implementation |
|-------|----------------|
| Interface | Mobile **PWA** (`static/`) + HTTP API |
| Orchestration | **LangGraph** state machine (stdlib ReAct fallback) |
| Models | OpenAI / Anthropic via env (OpenRouter/Groq compatible base URLs) |
| Tools | Filesystem, ripgrep, safe shell, **VST harness** (`autopilot-compile.sh`) |
| Outer loop | **Grok Bot** ACP (`GROK_WEBHOOK_URL`, `POST /grok/send`) — see [GROK_PWA_INTEGRATION.md](../../docs/GROK_PWA_INTEGRATION.md) |
| Sandbox (fallback) | Local `.sandbox/`; optional **E2B** when `E2B_API_KEY` set |
| PWA team | Orchestrator → **Architect** → **Sandbox Coder** → **QA** (`POST /agent/team`) |
| MCP | Documented bridge — use Cursor/Zapier MCP on desktop; this service exposes REST tools |
| Observability | Optional Langfuse (`LANGFUSE_*`); structured JSON logs |

## Build & run

```bash
cd tools/coding-agent-stack
python3 -m venv .venv && source .venv/bin/activate
pip install -e ".[dev]"
cp .env.example .env   # set OPENAI_API_KEY or run in stub mode
uvicorn coding_agent_stack.main:app --reload --port 8790
```

Open **http://127.0.0.1:8790/** for the PWA chat (proxies to `/agent`).

**CLI (no server):**

```bash
python -m coding_agent_stack.cli "Run autopilot compile and summarize errors"
```

## Data flow

```text
Phone PWA ──POST /agent──► FastAPI
                              │
                              ▼
                    LangGraph (agent ◄──► tools)
                              │
              ┌───────────────┼───────────────┐
              ▼               ▼               ▼
        LocalSandbox    repo tools      run_harness_compile
              │               │               │
              └───────────────┴───────────────┘
                              ▼
                    tool output → next model turn
```

## Threading / realtime

HTTP handlers are async; tool execution uses `asyncio.to_thread` for subprocess/sandbox. No plugin audio-thread interaction.

## Key modules

| Module | Role |
|--------|------|
| `coding_agent_stack/graph.py` | LangGraph compile; fallback loop |
| `coding_agent_stack/tools.py` | Tool definitions + JSON schemas |
| `coding_agent_stack/sandbox.py` | Local + optional E2B code run |
| `coding_agent_stack/harness.py` | `scripts/vst/autopilot-compile.sh`, `error_log.txt` |
| `coding_agent_stack/main.py` | FastAPI, CORS, static PWA |
| `static/` | `manifest.json`, `sw.js`, chat UI |

## Extension points

- Add MCP client: spawn `npx @modelcontextprotocol/server-filesystem` and map to tool calls in `mcp_bridge.py`.
- GitHub PR reviewer: implement `POST /webhooks/github` with `GITHUB_WEBHOOK_SECRET`.
- Swap model: `AGENT_MODEL`, `OPENAI_BASE_URL` for Groq/OpenRouter.

## Related docs

- [docs/CURSOR_VST_ENGINEER.md](../../docs/CURSOR_VST_ENGINEER.md) — desktop VST rules + harness
- [vst-testing-ops/ARCHITECTURE.md](../../vst-testing-ops/ARCHITECTURE.md) — full CI pipeline
- [docs/CURSOR_AGENT_PLAYBOOK.md](../../docs/CURSOR_AGENT_PLAYBOOK.md) — Cloud Agent lane

## Deployment (free tier)

| Piece | Suggested host |
|-------|----------------|
| PWA + API | Render / Railway free web service (single process serves both) |
| LLM | Groq / OpenRouter free tier |
| E2B | E2B free sandbox minutes (optional) |

Set `CORS_ORIGINS` to your Vercel/Netlify URL in production.
