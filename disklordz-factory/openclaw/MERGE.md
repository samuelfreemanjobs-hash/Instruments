# Merge DiskLordz into `openclaw.json`

## 1. Install files

```bash
./disklordz-factory/scripts/install-openclaw-workspace.sh
```

This copies:

- `~/.openclaw/workspaces/disklordz_maestro/` … (ensemble + artist lanes)
- `~/.openclaw/agents/DISKLORDZ_*` (factory role aliases)
- `~/.openclaw/agents.config.disklordz.json` (reference)

## 2. Merge agent entries

Open your Gateway config (commonly `~/.openclaw/openclaw.json`). Merge **`agents.entries`** from `agents.config.disklordz.json` into your file’s top-level **`agents`** object.

Keep your existing `agents.defaults` or adopt DiskLordz defaults:

```json
"defaults": {
  "model": "anthropic/claude-sonnet-4-6",
  "sandbox": { "mode": "all", "scope": "agent" }
}
```

Set **`disklordz_maestro`** as `"default": true` when this collective should own the Gateway.

## 3. Example → DiskLordz mapping

| Your ensemble example | DiskLordz config entry | Factory / OpenClaw role |
|----------------------|------------------------|-------------------------|
| `bandleader` | `disklordz_maestro` | Orchestrator (`DISKLORDZ_FACTORY_ORCHESTRATOR`) |
| `keys_arranger` | `vintage_keys` | Harmony / keys briefs |
| `rhythm_section` | `rhythm_section` | Drums / 808 / screw pocket |
| `vocal_topline` | `vocal_chops` | Soul chops & vocal stems |
| — | `market_intel`, `aar` | Research gate |
| — | `artist_dl001` … `004`, `006` | HQ greenlit lane guardians |

Persona names (**Isaac, Marcus, Bernard, Evelyn**) are preserved on the maestro and section leaders.

## 4. Validate

```bash
curl -s http://127.0.0.1:8787/research/status
curl -s http://127.0.0.1:8787/collective/artist-agents
```

Restart Gateway after config merge.

## 5. Gateway routing (spawn / isolation)

Read **`routing.config.disklordz.json`** (installed next to your state dir) and [docs/OPENCLAW_GATEWAY_ROUTING.md](../../docs/OPENCLAW_GATEWAY_ROUTING.md).

- Maestro: Opus + `sessions_spawn` + `exec`
- Section leaders: Sonnet, **deny** `sessions_spawn` — forked threads hold harmonic/drum/vocal depth
- Default spawn: **`executionContext: fork`**; Maestro merges ≤400-token JSON summaries only
