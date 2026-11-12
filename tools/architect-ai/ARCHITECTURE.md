# ArchitectAI — architecture

## Purpose

Standalone CLI agent **ArchitectAI**: principal architect / DevOps mentor using the **COSI** framework (Components, Organization, State, Interfaces). Targets local design reviews and curriculum-style teaching without coupling to the Disklordz profit-agent fleet.

## Build & run

```bash
cd tools/architect-ai
pip install -r requirements.txt
cp .env.example .env
python architect_ai/cli.py
python scripts/smoke.py
```

## Data flow

```text
cli.py
  → load .env (optional) + pick provider (gemini | openai)
  → ArchitectSession(system prompt, message history)
  → optional context.py: read --review paths (byte cap, skip secrets/binaries)
  → ChatModel.chat(system, messages)
  → stdout (REPL or --once)
```

Hexagonal layout:

| Layer | Path | Role |
|-------|------|------|
| Domain | `prompts.py`, `session.py` | Persona, COSI appendix, turn orchestration |
| Port | `adapters/base.py` | `ChatModel` protocol |
| Adapters | `gemini.py`, `openai_chat.py`, `langchain_gemini.py` | Vendor APIs |
| Infrastructure | `context.py`, `cli.py` | Filesystem context, REPL |

## Threading / state

Single-process CLI; conversation state lives in memory until `/reset` or exit. No server, no persistence.

## Extension points

- Add `review` subcommand with git diff ingestion.
- Wire LangChain tools (repo search) behind the same `ChatModel` port.
- Register in CI as `--print-system` + import smoke only (no API key in CI).

## Related docs

- [README.md](README.md)
- [docs/CURSOR_AGENT_PLAYBOOK.md](../../docs/CURSOR_AGENT_PLAYBOOK.md)
- Root [ARCHITECTURE.md](../../ARCHITECTURE.md)
