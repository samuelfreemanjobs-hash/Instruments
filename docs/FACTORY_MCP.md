# Factory Music MCP — tool execution layer

Implements the **MCP skills** row of the [OpenClaw ensemble stack](OPENCLAW_ENSEMBLE_STACK.md): scale/BPM checks, MIDI map shells, and **`DL-BRF`** brief validation.

## Cursor (this repo)

```bash
cd disklordz-factory/integrations/factory_music_mcp
python3 -m venv .venv && source .venv/bin/activate && pip install -r requirements.txt
```

`.cursor/mcp.json` includes **`factory-music`** pointing at `server.py`.

Restart Cursor MCP after install.

## OpenClaw Gateway

Add an MCP server entry in Gateway config (path to this repo’s venv python + `server.py`). Grant tool access to:

- `disklordz_maestro` — validate merged briefs
- `vintage_keys`, `rhythm_section`, `vocal_chops` — BPM + MIDI shells (fork sessions)

Do **not** grant Factory MCP `exec` replacement for greenlight — research stays on Factory HTTP API.

## Tools (summary)

- **`check_artist_bpm`** — DL001 118–124, DL002 140–160, DL006 60–95, DL004 112–128  
- **`validate_dl_brf`** — required fields per [`dl_brf.json`](../disklordz-factory/schemas/dl_brf.json)  
- **`format_midi_map_shell`** — sectional output convention  
- **`compile_artist_prompt`** — loads `prompts/artists/DL*.md`

## Related

- [OPENCLAW_GATEWAY_ROUTING.md](OPENCLAW_GATEWAY_ROUTING.md)
- [integrations/factory_music_mcp/README.md](../disklordz-factory/integrations/factory_music_mcp/README.md)
