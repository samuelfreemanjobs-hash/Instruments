# Disklordz — Cursor MCP (not Activepieces)

Use **Cursor MCP** for PM + notifications. Keep **factory audio** in `tools/` and **sync script/CI** for bulk Airtable writes when MCP is slow.

## 1. Enable servers in Cursor

**Settings → MCP** (or project `.cursor/mcp.json`).

### Airtable (PM registry)

Project file: [`.cursor/mcp.json`](../../.cursor/mcp.json)

1. Create token: [airtable.com/create/tokens](https://airtable.com/create/tokens) → base `appUuhVQHAOv31wJ1` → scopes: read/write records.
2. In Cursor MCP config for **airtable**, set `AIRTABLE_TOKEN` to your `pat…`  
   (Do **not** commit the token; use Cursor’s MCP env UI or local override.)
3. Reload MCP → confirm **airtable** tools list (list records, create, update).

Built-in **Slack** MCP (if connected): batch announcements. No Activepieces required.

## 2. Agent instructions

- Rule: [`.cursor/rules/disklordz-pm-agent.mdc`](../../.cursor/rules/disklordz-pm-agent.mdc) — enable for PM chats.
- Full spec: [DISKLORDZ_AUDIO_PM_AGENT.md](DISKLORDZ_AUDIO_PM_AGENT.md)

## 3. Minimal flows (paste in Cursor chat)

### Flow A — First run (registry)

```
PM ADD: Disklordz — phonk factory & GRAVEYARD_SHIFT v1
[use fields from AIRTABLE_SEED.md]

PM UPDATE: Disklordz — Audio PM Agent
[rebrand from PM Agent — Proactive Project Intelligence per AIRTABLE_SEED.md]

PM BRIEFING
```

Agent must **call Airtable MCP** to read/write `Projects`, not invent rows.

### Flow B — After factory batch

```bash
cd tools && python3 render_kit.py --kit-name GRAVEYARD_SHIFT_808S --mode grid808 --count 10
```

Then in Cursor:

```
PM UPDATE: Disklordz — phonk factory & GRAVEYARD_SHIFT v1
Rendered 10 more 808s. % Complete now 35. Next Action: listen and cull weak kicks.
```

Optional Slack MCP (confirm before send):

```
Post to #disklordz (or your channel): Disklordz factory batch done — 10 new 808s in tools/output/samples.
```

### Flow C — Weekly ops

```
PM BRIEFING
```

Filter in chat: “only Disklordz and audio projects” if the base still has legacy Freeman rows.

## 4. When to use GitHub Action instead of MCP

| Use MCP (Cursor) | Use `scripts/sync_disklordz_pm_to_airtable.py` + GHA |
|------------------|------------------------------------------------------|
| Interactive PM UPDATE / BRIEFING | Bootstrap many fields idempotently |
| One-off adds while you work | CI on push when `AIRTABLE_API_KEY` repo secret exists |

Repo secret: GitHub → Settings → Secrets → `AIRTABLE_API_KEY` → re-run workflow **Sync Disklordz PM to Airtable**.

## 5. Cloud Agent note

Pods may block `api.airtable.com` until **egress allowlist** includes it (see `.cursor/environment.json`). Local Cursor + MCP uses **your** network and works once the token is set.

## 6. What we are not doing

- **Activepieces / Zapier** for PM (optional later for storefront only).
- **Agent Zero** orchestrators.
- MCP driving `render_kit.py` loops — you run the CLI; MCP updates **registry + Slack**.
