# DiskLordz OpenClaw workspace — agent routing

Primary orchestrator for all vintage collective batches:

- **Default agent:** `DISKLORDZ_FACTORY_ORCHESTRATOR`

## HQ mission

Autonomous vintage sample synthesis: **1970s–1980s soul, jazz, and R&B** source behavior → four commercial lanes (French touch, **Southern fast phonk**, **screw / 90s phonk**, cyber funk).

## Greenlit artist lanes (only)

- `DISKLORDZ_ARTIST_DL001` — Boulevard 86
- `DISKLORDZ_ARTIST_DL002` — Midnight Circuit (drift/wave phonk)
- `DISKLORDZ_ARTIST_DL006` — Disklordz (DJ Screw / slow phonk)
- `DISKLORDZ_ARTIST_DL004` — Terminal Mirage

## Worker spawn order (typical batch)

1. `DISKLORDZ_MARKET_INTEL` → Factory `POST /research/opportunities` or read existing `DL-OPP-*`
2. `DISKLORDZ_AAR` → greenlight per artist (Factory research API)
3. For each greenlit `artist_id`: spawn artist guardian + `DISKLORDZ_MUSIC_PRODUCER` + `DISKLORDZ_SOUND_DESIGNER`
4. `DISKLORDZ_SAMPLE_MINER` after track briefs approved
5. `DISKLORDZ_VISUAL_DIRECTOR`, `DISKLORDZ_CONTENT_PRODUCER`
6. `DISKLORDZ_RIGHTS_QA` before `workboard_complete`

## Tools

- OpenClaw Workboard: `workboard_create`, `workboard_link`, `workboard_dispatch`, `workboard_complete`
- Factory API base: `http://127.0.0.1:8787` (health, research, collective, night-shift)

## Never

- Publish market demand without citations
- Name artists in external copy before `ready_for_production`
- Mix DL002 fast phonk with DL006 screw on one card
