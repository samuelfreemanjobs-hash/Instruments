# Coding Agent Stack

Custom **LangGraph-shaped ReAct agent** + **FastAPI** + **mobile PWA** for off-desk tasks in the Instruments monorepo.

## Roles vs your other tools

| Tool | Best for |
|------|-----------|
| **VS Code / plain Cursor editor** | Typing, debugging, UI |
| **Claude Code** | Terminal git, quick refactors |
| **This PWA agent** | Async harness runs, sandbox scripts, grep/read, draft summaries from your phone |

Without Cursor Agent Mode, this stack is your **autopilot layer**: it runs `./scripts/vst/autopilot-compile.sh` and reads `vst-testing-ops/error_log.txt` via tools.

## Quick start

See [ARCHITECTURE.md](ARCHITECTURE.md).

## Free cloud

Deploy the same `uvicorn` app on Render/Railway free tier; point the PWA at that URL (update fetch in `static/assets/app.js` or serve from the same origin). Use Groq/OpenRouter free models via `OPENAI_BASE_URL` + `OPENAI_API_KEY`.
