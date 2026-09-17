# OpenClaw 2.0 — vintage collective multi-agent architecture

DiskLordz maps the **1970s–1980s soul, jazz, and R&B collective → Southern phonk & screwed production** (plus French touch and cyber funk lanes) onto **OpenClaw 2.0** as an asynchronous, local-first LLM runtime with persistent state, tool APIs, and file-based identity.

## Runtime model (from OpenClaw)

| Property | Role in DiskLordz |
|----------|-------------------|
| **Local-first Gateway** | Dispatches subagent workers; Workboard cards link to session keys |
| **SQLite persistence** | OpenClaw-owned (Gateway plugin store) — separate from Factory in-memory API |
| **SOUL.md** | Agent persona, non-negotiable lane ethics, vintage aesthetic |
| **AGENTS.md** | Routing: who to delegate to, tools, spawn rules |
| **Workboard** | Kanban decomposition: parent batch → per-artist → per-stage children |
| **Heartbeat / Automations** | Night shift triggers (Factory cron or `openclaw workboard dispatch`) |

Factory API ([`disklordz-factory/`](../disklordz-factory/)) holds **catalog, research gate, approval queue**. OpenClaw holds **execution orchestration**.

## Hierarchical topology

```text
                    ┌─────────────────────────────────────┐
                    │ DISKLORDZ_FACTORY_ORCHESTRATOR      │
                    │ (primary — batch CEO, decompose)    │
                    └──────────────┬──────────────────────┘
           ┌───────────────────────┼───────────────────────┐
           │                       │                       │
    ┌──────▼──────┐         ┌───────▼───────┐       ┌───────▼────────┐
    │  Research   │         │  Production   │       │  Artist lane   │
    │  workers    │         │  workers      │       │  guardians (×4)│
    └─────────────┘         └───────────────┘       └────────────────┘
```

### Tier 1 — Orchestrator (single primary)

- **ID:** `DISKLORDZ_FACTORY_ORCHESTRATOR`
- **Job:** Accept HQ mission → create Workboard parent card → `workboard_decompose` into children → `workboard_dispatch` (≤3 workers/pass default).
- **Never:** Renders audio, invents market stats, greenlights without `DL-OPP-*`.

Template: [`disklordz-factory/openclaw/agents/DISKLORDZ_FACTORY_ORCHESTRATOR/`](../disklordz-factory/openclaw/agents/DISKLORDZ_FACTORY_ORCHESTRATOR/).

### Tier 2 — Factory workers (background specialists)

| OpenClaw agent ID | Factory role |
|-------------------|--------------|
| `DISKLORDZ_MARKET_INTEL` | Sourced `DL-OPP-*` only |
| `DISKLORDZ_AAR` | Greenlight / reject with rationale |
| `DISKLORDZ_MUSIC_PRODUCER` | Vintage synthesis briefs |
| `DISKLORDZ_SOUND_DESIGNER` | Drum/chop families |
| `DISKLORDZ_SAMPLE_MINER` | Crate split kits |
| `DISKLORDZ_VISUAL_DIRECTOR` | Brand visuals |
| `DISKLORDZ_CONTENT_PRODUCER` | YouTube metadata |
| `DISKLORDZ_RIGHTS_QA` | Provenance gate |

Contracts in [`disklordz-factory/agents/`](../disklordz-factory/agents/). OpenClaw **SOUL/AGENTS** templates under [`disklordz-factory/openclaw/agents/`](../disklordz-factory/openclaw/agents/) (orchestrator + research + production stubs).

### Tier 2b — Artist lane guardians (HQ greenlit)

Parallel **routing consultants** — each validates work for one YouTube brand:

| OpenClaw ID | Artist | Brand | Southern / screw note |
|-------------|--------|-------|------------------------|
| `DISKLORDZ_ARTIST_DL001` | DL001 | Boulevard 86 | *Not* phonk — disco-funk → French touch |
| `DISKLORDZ_ARTIST_DL002` | DL002 | Midnight Circuit | **Fast** phonk (drift/wave) |
| `DISKLORDZ_ARTIST_DL006` | DL006 | Disklordz | **Screw / 90s phonk** (slow South) |
| `DISKLORDZ_ARTIST_DL004` | DL004 | Terminal Mirage | Cyber funk; vapor visual only |

Templates: [`disklordz-factory/openclaw/agents/DISKLORDZ_ARTIST_DL*/`](../disklordz-factory/openclaw/agents/).

Producer and Sample Miner workers **must** load the artist guardian context for the card’s `artist_id` before completing `workboard_complete`.

## Workboard card graph

1. **Parent:** `Vintage collective batch — {batch_id}` (`workflows/openclaw_vintage_collective_cards.json`).
2. **Per artist (×4):** DISCOVER → IDEATE → GENERATE → PRODUCE → PACKAGE → PUBLISH → QA.
3. **Dependencies:** QA blocks publish externally; Rights QA blocks Factory approval queue.

Dispatch CLI: `openclaw workboard dispatch` (Gateway). Fallback simulation: `POST /night-shift/run` on Factory API.

## Install workspace into OpenClaw

From repo root:

```bash
./disklordz-factory/scripts/install-openclaw-workspace.sh
# optional target:
OPENCLAW_STATE_DIR=~/.openclaw ./disklordz-factory/scripts/install-openclaw-workspace.sh
```

Copies:

- `openclaw/workspaces/*` → `~/.openclaw/workspaces/` (Isaac/Maestro + section leaders + artist lanes)
- `openclaw/agents/DISKLORDZ_*` → `~/.openclaw/agents/`
- `agents.config.disklordz.json` → merge into `openclaw.json` — see [`openclaw/MERGE.md`](../disklordz-factory/openclaw/MERGE.md)

### Gateway `agents.entries` (ensemble pattern)

Same shape as a vintage **bandleader + section** config:

| Entry key | Display name | Role |
|-----------|--------------|------|
| `disklordz_maestro` | Isaac "The Maestro" Vance | Default orchestrator, `sessions_spawn`, Opus |
| `vintage_keys` | Marcus "Hertz" Dupree | Keys/Rhodes/FM |
| `rhythm_section` | Bernard "Pocket" Washington | Drums / phonk / screw pocket |
| `vocal_chops` | Evelyn "Velvet" Hayes | Soul chops & stems |
| `artist_dl001` … | DISKLORD 001–004/006 | Lane guardians |
| `market_intel`, `aar` | Research gate | `DL-OPP-*` only |

Full JSON: [`disklordz-factory/openclaw/agents.config.disklordz.json`](../disklordz-factory/openclaw/agents.config.disklordz.json).

Restart Gateway after merge; default agent: **`disklordz_maestro`**.

## Research gate (unchanged)

OpenClaw agents **read** Factory research state:

```bash
curl -s http://127.0.0.1:8787/research/status
curl -s http://127.0.0.1:8787/research/greenlights
curl -s http://127.0.0.1:8787/collective/artist-agents/DL006
```

HQ greenlit artists: [ARTIST_GREENLIGHTS.md](ARTIST_GREENLIGHTS.md).

## Related

- [FACTORY_VINTAGE_COLLECTIVE.md](FACTORY_VINTAGE_COLLECTIVE.md)
- [OpenClaw Workboard plugin](https://docs.openclaw.ai/plugins/workboard)
- [`disklordz-factory/openclaw/README.md`](../disklordz-factory/openclaw/README.md)
