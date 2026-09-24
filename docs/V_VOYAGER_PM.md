# V Voyager — PM registration (Airtable Disklordz OS)

**Product ID:** `DL-PLUGIN-V-VOYAGER`  
**VST3 bundle:** `V Voyager.vst3`  
**Code path:** `plugin-factory/plugins/moog-voyager/`  
**Implementer agent:** `cursor-v-voyager` (Cursor Cloud)

## Import into Airtable

1. Open base **Disklordz OS** (PM source of truth — not Linear).
2. Import or paste rows from [`disklordz/airtable/seed/v-voyager-2026.json`](../disklordz/airtable/seed/v-voyager-2026.json):
   - **Products** → `DL-PLUGIN-V-VOYAGER`
   - **Projects** → `PRJ-2026-V-VOYAGER`
   - **Agent roster** → `cursor-v-voyager` (add to your Agents / Team table if you maintain one)
   - **Agent Work Orders** → `WO-2026-VVOY-*`
3. Set Marketing / Planner gates on `WO-2026-VVOY-003` before any store SKU.

## Team agent: `cursor-v-voyager`

| Field | Value |
|-------|--------|
| **Owner** | Cursor Cloud Agent on `samuelfreemanjobs-hash/Instruments` |
| **Scope** | Plugin Factory OS + V Voyager DSP, presets, CI ship |
| **Rules** | `AGENTS.md`, `.cursor/rules/cursor-v-voyager-agent.mdc`, `plugin-factory/plugins/moog-voyager/README.md` |
| **Handoff inbox** | `disklordz/cursor-cloud/inbox/CO-*.json` |
| **WIP** | Counts toward Factory Manager **max 2** Cursor JUCE WOs when assigned implementation work |

Paste this into a new **Cursor Cloud Agent** automation (Dashboard → Automations):

```markdown
You are cursor-v-voyager on Instruments. Read ARCHITECTURE.md, plugin-factory/ARCHITECTURE.md,
plugin-factory/plugins/moog-voyager/README.md. For V Voyager work: cd plugin-factory &&
./scripts/factory.sh release before push. Branch: cursor/<task>-<suffix>.
```

## Scheduled automations (required)

| Schedule (UTC) | System | What runs |
|----------------|--------|-----------|
| Daily 11:00 | GitHub `Nightly QA` | `run_business.py --profile full` (includes factory build + ship) |
| Mon/Wed/Fri 12:00 | GitHub `Scheduled Plugin Factory QA` | `factory.sh release` + assert **V Voyager.vst3** |
| On demand | Airtable → GitHub | `repository_dispatch` → `airtable-cursor-cloud-handoff` (see below) |

### Airtable automation (Cursor handoff)

1. **Trigger:** `Agent Work Orders` → `status` is **Ready for Cursor Cloud** (create this status if missing).
2. **Filter:** `owner_agent` = `cursor-v-voyager` **OR** `title` contains `[Plugin][Factory]`.
3. **Action:** Webhook POST to GitHub:

```http
POST https://api.github.com/repos/samuelfreemanjobs-hash/Instruments/dispatches
Authorization: Bearer <GITHUB_PAT repo scope>
Accept: application/vnd.github+json

{
  "event_type": "airtable-cursor-cloud-handoff",
  "client_payload": {
    "work_order_id": "WO-2026-VVOY-002",
    "branch": "main"
  }
}
```

Workflow: [`.github/workflows/airtable-cursor-cloud-handoff.yml`](../.github/workflows/airtable-cursor-cloud-handoff.yml)

Secrets: `AIRTABLE_API_KEY`, `AIRTABLE_BASE_ID` (same as Antigravity lane).

## Verify ship

```bash
cd plugin-factory
./scripts/factory.sh release
test -d build/plugins/moog-voyager/MoogVoyager_artefacts/Release/VST3/V\ Voyager.vst3
```

## Related

- Plugin track map: [DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md)
- Automation hub: [disklordz/automation/README.md](../disklordz/automation/README.md)
- Preset catalog: [plugin-factory/plugins/moog-voyager/PRESETS.md](../plugin-factory/plugins/moog-voyager/PRESETS.md)
