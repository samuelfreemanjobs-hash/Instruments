# AI coding agent stack (custom + mobile)

**Production mobile path:** [GROK_PWA_INTEGRATION.md](GROK_PWA_INTEGRATION.md) — Next.js `/remote-agent` webhooks your **existing Grokbot** (no Render Python backend).

This repo also ships an **optional** local harness under `tools/coding-agent-stack/` for dry-run ACP payloads and VST compile smoke tests — not required when Grok Bot is the cloud backend.

## Where you use it

| Surface | URL / command |
|---------|----------------|
| **Phone PWA** | Deploy FastAPI, open `/`, Add to Home Screen |
| **Laptop API** | `http://127.0.0.1:8790/agent` |
| **CLI** | `python -m coding_agent_stack.cli "run agent tests"` |

## Stack layers (mapped)

1. **Agents & UI** — FastAPI + static PWA (`static/`)
2. **Models** — `OPENAI_API_KEY` / `ANTHROPIC_API_KEY` (Groq/OpenRouter via `OPENAI_BASE_URL`)
3. **Orchestration** — LangGraph optional; built-in ReAct loop in `graph.py`
4. **Context** — [VST_AI_PLUGIN_MAP.md](VST_AI_PLUGIN_MAP.md), `read_file` / `grep_repo` tools
5. **Outer loop** — **Grok Bot** via ACP/webhook ([GROK_PWA_INTEGRATION.md](GROK_PWA_INTEGRATION.md)); E2B optional fallback
6. **Harness** — `harness_vst_compile` → `scripts/vst/autopilot-compile.sh`
7. **Observability** — optional Langfuse (`pip install '.[obs]'`)

## Best role when you skip Cursor IDE

You lose inline autopilot; this agent becomes the **Remote Code Generator & PR buddy**:

- Kick off **long compile/test loops** from your phone
- **Sandbox** untrusted scripts (E2B or local)
- **Grep/read** the monorepo without opening the IDE

Keep **Claude Code** on desktop for git and single-file edits.

## 100% free cloud

| Layer | Provider |
|-------|----------|
| PWA + API | Render / Railway free web service |
| LLM | Groq / OpenRouter free models |
| Sandbox | E2B free tier (optional) |

Wake-up delay on Render free tier (~30–50s) is expected.

## Implementation doc

[tools/coding-agent-stack/ARCHITECTURE.md](../tools/coding-agent-stack/ARCHITECTURE.md)
