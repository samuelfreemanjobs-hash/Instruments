# Claude — Disklordz Audio PM Agent + Airtable MCP

One-time setup so Claude can run **`PM BRIEFING`** / **`PM ADD`** against your Projects table.

## 1. Personal Access Token

1. Open [airtable.com/create/tokens](https://airtable.com/create/tokens)
2. Name: `Disklordz Claude MCP`
3. Scopes: **data.records:read**, **data.records:write**, **schema.bases:read** (or full data access for this base)
4. Access: base **Freeman Intelligence — PM** (`appUuhVQHAOv31wJ1`) — rename to **Disklordz — PM** in UI when ready
5. Copy the token (`pat…`)

Use the **same** token value as your Cloud Agent secret `AIRTABLE_API_KEY` if you already created one.

## 2. Claude Desktop MCP config

**macOS:** `~/Library/Application Support/Claude/claude_desktop_config.json`  
**Windows:** `%APPDATA%\Claude\claude_desktop_config.json`

If the file already has `mcpServers`, **merge** the `airtable` block from:

[claude-desktop-mcp.disklordz.json](claude-desktop-mcp.disklordz.json)

Replace `PASTE_YOUR_AIRTABLE_PERSONAL_ACCESS_TOKEN` with your PAT.

**Quit Claude fully** (Cmd+Q / Alt+F4), then reopen.

Verify: Claude → Settings → Developer → MCP → **airtable** connected.

## 3. Claude Project

1. New Project: **Disklordz — Audio PM Agent**
2. **Project instructions:** paste entire file  
   [DISKLORDZ_AUDIO_PM_AGENT.md](DISKLORDZ_AUDIO_PM_AGENT.md)
3. Enable MCP **airtable** for this project (if prompted)

## 4. First commands (paste in project chat)

```
PM ADD: Disklordz — phonk factory & GRAVEYARD_SHIFT v1

Urgency HIGH. Status ACTIVE. Phase PHASE-1. 25% complete.
Scope: Active math factory tools/render_kit.py, GRAVEYARD_SHIFT kit v1, docs/DISKLOrdZ_FACTORY_PLAN.md in Instruments repo.
Next Action: Listen to GRAVEYARD_SHIFT batch in DAW; sort 26 WAVs into kit folders.
```

```
PM UPDATE: Disklordz — Audio PM Agent

Renamed from PM Agent — Proactive Project Intelligence. For Who: Sam / Disklordz audio ops. 85% complete. Health GREEN.
```

```
PM BRIEFING
```

## 5. Sync from terminal (no Claude)

If MCP is not ready, from repo root with `AIRTABLE_API_KEY` set:

```bash
python3 scripts/sync_disklordz_pm_to_airtable.py
```

## 6. Cursor (optional)

Same MCP block can go in Cursor **Settings → MCP** (stdio server). Use env var for token; do not commit secrets.
