# Isaac — compile protocol (JSON → unified prompt)

## Input (from forks)

Three files or spawn results, each validating against `schemas/dl_section_payload.json`:

- `keys_section.json` — `agent_identifier: keys_arranger`
- `rhythm_section.json` — `agent_identifier: rhythm_section`
- `vocal_section.json` — `agent_identifier: vocal_topline`

Set `movement_target` per lane (each artist has full lane doc under `docs/lanes/`):

| artist_id | Brand | movement_target | Required `compiled_music_prompt.downstream` |
|-----------|-------|-----------------|---------------------------------------------|
| DL001 | Boulevard 86 | `french_touch` | `downstream.french_touch` |
| DL002 | Midnight Circuit | `memphis_90s`, `cloud_phonk` | both blocks when dual brief |
| DL006 | Disklordz | `screw` | `downstream.screw` |
| DL004 | Terminal Mirage | `cyber_funk` | `downstream.cyber_funk` (+ `vapor_aesthetic` meta) |

Registry: `database/artist_lane_processing.json` · Index: `docs/ARTIST_LANE_BIBLE.md`

## Merge output

Write `compiled_music_prompt.json` per `schemas/compiled_music_prompt.json`.

`compiled_prompt_text` must embed:

1. Neve 8068 / Studer A800 color
2. Section payloads **as facts**, not chat paraphrase
3. `downstream` block from `database/sonic_archaeology_movements.json` when movement is screw / memphis / cloud_phonk

## Anti-drift rules

- Do not invent section content not present in JSON payloads
- Do not paste `markdown_notes` verbatim — extract facts only
- Reject merge if any section missing required payload fields (MCP: `validate_section_payload`)

## Storage

`~/.openclaw/workspaces/disklordz_maestro/briefs/compiled_music_prompt_{batch_id}.json`

Factory: attach as `DL-BRF-*` metadata when approved.
