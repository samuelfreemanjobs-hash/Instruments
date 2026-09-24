# Google Stitch MCP (Cursor)

Use **Stitch** to iterate Night Circuit UI mockups from [../design/DESIGN.md](../design/DESIGN.md) and export alignment notes for JUCE (`Source/PluginEditor.cpp`).

## 1. Repo config

Root [`.cursor/mcp.json`](../../.cursor/mcp.json) includes a **stitch** HTTP MCP server. **No secrets in git** — set:

| Variable | Purpose |
|----------|---------|
| `STITCH_API_KEY` | API key from Stitch / Google Labs (see [Stitch MCP setup](https://stitch.withgoogle.com/docs/mcp/setup)) |
| `STITCH_ACCESS_TOKEN` | Optional OAuth bearer (alternative to API key) |
| `GOOGLE_CLOUD_PROJECT` | Required with OAuth; enable `stitch.googleapis.com` |

Restart Cursor after exporting env vars so `${env:STITCH_API_KEY}` resolves.

## 2. Connect in Cursor

1. **Settings → MCP** → enable **stitch** from this repo’s `mcp.json`.
2. If generation tools fail with auth errors, use OAuth per official docs (some tools require bearer token, not key-only).

## 3. Useful MCP tools

| Tool | Use for Night Circuit |
|------|------------------------|
| `create_project` | New Stitch project for GUI iterations |
| `generate_screen_from_text` | Prompt from `DESIGN.md` § Stitch prompt seed |
| `get_screen` / `list_screens` | Pull layout references for Factory implementation |
| `list_projects` | Find existing Night Circuit design project |

## 4. Workflow with sound-design / Factory

1. Sound-design updates `DESIGN.md` tokens or prompt seed when palette changes.
2. Agent calls Stitch MCP to generate/refine screens (read-only exploration is fine; confirm before destructive Stitch writes).
3. **VST Plugin Factory** maps approved layout to JUCE `resized()` / `paint()` — Stitch output is reference, not runtime embed.

## 5. SDK (optional, local scripts)

```bash
npm install @google/stitch-sdk
STITCH_API_KEY=... node scripts/stitch-export-night-circuit.mjs  # add script if needed
```

Official package: [@google/stitch-sdk](https://www.npmjs.com/package/@google/stitch-sdk).

## Related

- [NIGHT_CIRCUIT_GUI_SPEC.md](../design/NIGHT_CIRCUIT_GUI_SPEC.md)
- [PRESET_BROWSER_GUI.md](PRESET_BROWSER_GUI.md)
