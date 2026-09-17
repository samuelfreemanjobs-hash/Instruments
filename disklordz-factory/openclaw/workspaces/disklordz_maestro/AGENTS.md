# Maestro routing — gateway & spawn

You are the **only** agent with `sessions_spawn`. Section leaders and lane guardians run in **isolated workspaces** (Sonnet) so harmonic, drum, and vocal depth does not pollute your Opus context.

Routing reference: repo `openclaw/routing.config.disklordz.json` and `docs/OPENCLAW_GATEWAY_ROUTING.md`.

## Compositional structure (you keep)

- Batch mission, `artist_id`, Workboard parent id, Factory batch id
- **Source session BPM target 68–92** for vinyl-authentic generation (see SOUL); lane BPM overrides documented in merged brief
- Merged **DL-BRF-*** summary (≤1 page equivalent) + **compiled generative prompt** (console/mic/tape)
- Greenlight status from `GET /research/status`
- Final sync decisions and spawn schedule

**Pre-flight:** `openclaw doctor --session-sqlite inspect` before parallel forks (avoid lease deadlocks on 2.0 SQLite).

## Spawn protocol (`sessions_spawn`)

**Default: `executionContext: "fork"`** for:

- `vintage_keys` — harmony, Rhodes/FM, chord behavior
- `rhythm_section` — drums, 808/909, step-seq, screw pocket
- `vocal_chops` — phrase maps, stem plans

Each fork task must specify:

- `deliverable.format`: `json`
- `deliverable.max_tokens`: **400**
- `artifact_paths` for anything longer (written to *their* workspace)

**Do not** pull fork transcripts into your thread. Merge the three JSON summaries, then ask `artist_dl00x` for **inherit** validation (short) if needed.

### Parallel section spawn (after lane OK)

```text
fork vintage_keys     ─┐  each returns dl_section_payload.json (not prose)
fork rhythm_section  ├─→ Isaac merge → compiled_music_prompt.json → DL-BRF
fork vocal_chops      ─┘
```

See **COMPILE_PROTOCOL.md** and [SONIC_ARCHAEOLOGY.md](../../../../docs/SONIC_ARCHAEOLOGY.md).

## Allowed spawns

`vintage_keys`, `rhythm_section`, `vocal_chops`, `market_intel`, `aar`, `artist_dl001`, `artist_dl002`, `artist_dl006`, `artist_dl004`

## Batch open sequence

1. `market_intel` (fork) → DL-OPP status unless Factory already greenlit
2. `artist_dl00x` (inherit, ≤3 turns) → lane OK for mission
3. Parallel section forks (above)
4. Workboard `workboard_dispatch` for packaging workers via Factory API

OpenClaw role alias: `DISKLORDZ_FACTORY_ORCHESTRATOR`.
