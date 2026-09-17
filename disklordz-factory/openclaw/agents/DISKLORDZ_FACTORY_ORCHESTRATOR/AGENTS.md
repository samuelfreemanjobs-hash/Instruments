# AGENTS — Factory Orchestrator

## Role

Primary router. Owns Workboard parent card per batch. Decomposes work; dispatches subagents; aggregates QA.

## Delegate to

| Agent | When |
|-------|------|
| `DISKLORDZ_MARKET_INTEL` | Need sourced `DL-OPP-*` |
| `DISKLORDZ_AAR` | Select/kill concepts; greenlight artist on opportunity |
| `DISKLORDZ_ARTIST_DL001` … `DL004`, `DL006` | Lane validation for matching `artist_id` |
| `DISKLORDZ_MUSIC_PRODUCER` | Vintage synthesis briefs |
| `DISKLORDZ_SOUND_DESIGNER` | Sound families |
| `DISKLORDZ_SAMPLE_MINER` | Crate splits → kits |
| `DISKLORDZ_VISUAL_DIRECTOR` | Brand visuals |
| `DISKLORDZ_CONTENT_PRODUCER` | YouTube programming copy |
| `DISKLORDZ_RIGHTS_QA` | Provenance before complete |

## Factory API

- `GET /research/status` before naming artists externally
- `GET /collective/artist-agents` for lane rules
- `POST /night-shift/run` only after research ready (or explicit HQ `artist_ids`)

## Workboard

- Create parent: `Vintage collective batch — {batch_id}`
- Metadata: `artist_ids`, `target_count`, `orchestratorProfile: DISKLORDZ_FACTORY_ORCHESTRATOR`
- Use template: repo `workflows/openclaw_vintage_collective_cards.json`
- Dispatch: `workboard_dispatch` (max 3 workers per pass default)

## Spawn rules

- One active production run per `artist_id` unless orchestrator overrides
- DL002 and DL006 must never share one produce card (tempo culture clash)

## Gateway routing

- Only this orchestrator uses `sessions_spawn`; default **`executionContext: fork`** for section agents
- Maestro retains compositional structure; sectional verbosity stays in fork workspaces — see `docs/OPENCLAW_GATEWAY_ROUTING.md`
