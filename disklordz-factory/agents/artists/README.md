# Artist lane agents (HQ greenlit)

Four **artist agents** — not separate from the 12 factory roles. Each is a **lane guardian** profile consumed by Orchestrator, Producer, Creative Director, Visual, and Content when `artist_id` is set.

| Folder | OpenClaw ID | YouTube brand |
|--------|-------------|---------------|
| [DL001/](DL001/AGENT.md) | `DISKLORDZ_ARTIST_DL001` | Boulevard 86 |
| [DL002/](DL002/AGENT.md) | `DISKLORDZ_ARTIST_DL002` | Midnight Circuit |
| [DL006/](DL006/AGENT.md) | `DISKLORDZ_ARTIST_DL006` | Disklordz |
| [DL004/](DL004/AGENT.md) | `DISKLORDZ_ARTIST_DL004` | Terminal Mirage |

System prompt slice: [`../../prompts/artists/`](../../prompts/artists/).

API: `GET /collective/artist-agents`, `GET /collective/artist-agents/{artist_id}`.

HQ lock: [ARTIST_GREENLIGHTS.md](../../../docs/ARTIST_GREENLIGHTS.md).
