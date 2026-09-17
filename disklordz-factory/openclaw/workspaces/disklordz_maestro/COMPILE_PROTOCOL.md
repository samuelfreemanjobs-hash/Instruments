# Isaac — compile protocol (JSON → unified prompt)

## Input (from forks)

Three files or spawn results, each validating against `schemas/dl_section_payload.json`:

- `keys_section.json` — `agent_identifier: keys_arranger`
- `rhythm_section.json` — `agent_identifier: rhythm_section`
- `vocal_section.json` — `agent_identifier: vocal_topline`

Set `movement_target` per lane:

| artist_id | movement_target |
|-----------|-----------------|
| DL006 | `screw` |
| DL002 | `memphis_90s` and/or `cloud_phonk` |
| DL001 | `french_touch` |
| DL004 | `cyber_funk` |

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
