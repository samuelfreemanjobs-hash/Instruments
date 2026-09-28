# VST Plugin Factory — handoff lane

**Purpose:** Git-based assignment queue from **pm-agent** / **workflow-automation-agent** to **cursor-vst-plugin-factory** (JUCE implementer).

## Data flow

```
Airtable WO [Plugin][JUCE]
    → wo_to_vst_factory_handoff.py / GitHub Action
    → inbox/HO-*.json
    → Cursor Cloud Agent reads handoff + implements
    → PR (WO in title) + optional outbox/HO-*-done.json
```

## Build & run

Not a CMake target. Bridge CLI:

```bash
./scripts/vst-factory-bridge/vst-factory-bridge.sh send --wo WO-… --title "…" --push
```

Implementer build matrix: [AGENTS.md](../../AGENTS.md) JUCE section.

## Key paths

| Path | Role |
|------|------|
| `inbox/` | Open handoffs (`status: open`) |
| `outbox/` | Optional completion records |
| `AGENT_INSTRUCTIONS.md` | Inbox consumer steps |

## Related docs

- [docs/VST_PLUGIN_FACTORY_AGENT.md](../../docs/VST_PLUGIN_FACTORY_AGENT.md)
- [docs/VST_PLUGIN_FACTORY_SCHEDULE.md](../../docs/VST_PLUGIN_FACTORY_SCHEDULE.md)
- [disklordz/automation/VST_PLUGIN_FACTORY.md](../automation/VST_PLUGIN_FACTORY.md)

## Extension points

- Add Slack notify workflow on `inbox/HO-*.json` push (mirror Antigravity)
- Airtable fields: `vst_factory_handoff_id`, `owner_agent = cursor-vst-plugin-factory`
