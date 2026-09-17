# OpenClaw 2.0 workspace template — DiskLordz vintage collective

File-based agent identities for import into an OpenClaw Gateway state directory.

## Layout

```text
openclaw/
  AGENTS.md                 # Workspace-level routing index
  agents/
    DISKLORDZ_FACTORY_ORCHESTRATOR/
      SOUL.md
      AGENTS.md
    DISKLORDZ_MARKET_INTEL/
      SOUL.md
      AGENTS.md
    DISKLORDZ_AAR/
      SOUL.md
      AGENTS.md
    DISKLORDZ_MUSIC_PRODUCER/
      SOUL.md
      AGENTS.md
    DISKLORDZ_ARTIST_DL001/ … DL002, DL004, DL006/
      SOUL.md
      AGENTS.md
```

## Install

```bash
./disklordz-factory/scripts/install-openclaw-workspace.sh
```

Set `OPENCLAW_STATE_DIR` (default `~/.openclaw`) to match your Gateway.

## Factory coupling

| OpenClaw | Factory |
|----------|---------|
| Workboard cards | `workflows/openclaw_vintage_collective_cards.json` |
| Artist rules | `GET /collective/artist-agents/{id}` |
| Research gate | `GET /research/status` |
| Catalog / approval | `POST /night-shift/run`, approval queue |

Full architecture: [docs/OPENCLAW_VINTAGE_COLLECTIVE.md](../../docs/OPENCLAW_VINTAGE_COLLECTIVE.md).
