# Bytebot — local desktop agent (optional)

[Bytebot](https://github.com/bytebot-ai/bytebot) is an **open-source AI desktop agent**: full Linux desktop in Docker, web UI on port **9992**, mouse/keyboard automation for multi-app tasks.

**Not a substitute for Cursor Cloud** on this repo — use it for GUI/browser workflows Cursor Cloud does not run in your VM (vendor portals, desktop DAWs, one-off file ops).

## Requirements

- **Docker** + Docker Compose v2 (Docker Desktop on Mac/Windows)
- **One LLM provider key** (Anthropic, OpenAI, or Gemini)
- ~4 GB+ RAM for desktop + Postgres + agent + UI

**Cursor Cloud Agent pods do not include Docker** — run Bytebot on your **local machine** or a VPS with Docker.

## Quick start (official)

```bash
git clone https://github.com/bytebot-ai/bytebot.git
cd bytebot

# Add your AI provider key (choose one) — never commit this file
echo "ANTHROPIC_API_KEY=sk-ant-..." > docker/.env
# Or: echo "OPENAI_API_KEY=sk-..." > docker/.env
# Or: echo "GEMINI_API_KEY=..." > docker/.env

docker compose -f docker/docker-compose.yml up -d

# Web UI
open http://localhost:9992
```

### Services (default compose)

| Port | Service |
|------|---------|
| 9992 | **bytebot-ui** — task UI |
| 9991 | bytebot-agent — API |
| 9990 | bytebot-desktop — VNC / noVNC |
| 5432 | Postgres (bytebotdb) |

Compose reads `ANTHROPIC_API_KEY`, `OPENAI_API_KEY`, and `GEMINI_API_KEY` from `docker/.env` (see `docker/docker-compose.yml`).

## Disklordz / Instruments — when to use Bytebot

| Task | Better agent |
|------|----------------|
| SaaS code, PRs, factory, RAG | **Cursor Cloud** (`disklordz/website/`) |
| HISE plugin lane | **Antigravity** (Windows) + git handoff |
| Desktop browser ops, PDFs, multi-site downloads | **Bytebot** (local) |
| JUCE / CMake plugin | **Cursor** or **Cline** locally |

Example Bytebot tasks (ops, not repo edits):

- “Download stems from vendor portal X into `~/Imports`”
- “Screenshot Vercel preview and save to Desktop”
- “Fill this web form from the CSV on the desktop”

Commit results via **git** on your machine — Bytebot is not wired into `Instruments` CI.

## Security

- Keep `docker/.env` **local only**; do not paste keys into Slack or issues.
- Bytebot runs **privileged** desktop containers — use a dedicated machine or VM, not production secrets host.
- Prefer a **scoped** API key with spend limits.

## Alternatives

- [Railway one-click deploy](https://railway.com/deploy/bytebot) (hosted)
- Docs: https://docs.bytebot.ai/quickstart

## Related

- [DISKLORDZ_SAAS_AGENT_LANES.md](DISKLORDZ_SAAS_AGENT_LANES.md)
- [CURSOR_AGENT_PLAYBOOK.md](CURSOR_AGENT_PLAYBOOK.md)
