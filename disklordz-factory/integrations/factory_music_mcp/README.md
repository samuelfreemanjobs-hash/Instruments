# factory-music MCP (ensemble tool layer)

MCP skills for OpenClaw/Cursor sectional agents and Maestro:

| Tool | Role |
|------|------|
| `get_lane_constraints` | Lane rules from `artist_collective_seed.json` |
| `check_artist_bpm` | Scale / tempo gate |
| `validate_dl_brf` | Production brief schema + BPM |
| `format_midi_map_shell` | MIDI map JSON shell |
| `compile_artist_prompt` | Artist guardian prompt compile |
| `dl_brf_schema` | Raw `schemas/dl_brf.json` |

Setup: [docs/FACTORY_MCP.md](../../../docs/FACTORY_MCP.md).

OpenClaw: register this server in Gateway MCP settings so **`vintage_keys`** and **`rhythm_section`** forks can call tools without `exec`.
