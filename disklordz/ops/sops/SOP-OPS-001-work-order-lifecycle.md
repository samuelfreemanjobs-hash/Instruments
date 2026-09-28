# SOP-OPS-001 — Work order lifecycle

| Field | Value |
|-------|--------|
| **Owner seat** | hermes-ops |
| **Consumer seats** | hermes-lead, hermes-handoff, hermes-ops |
| **Cadence** | every_wo |

## Purpose

Move work from backlog → implementation → verified merge → Airtable Done without WIP chaos.

## Procedure

1. **Intake:** Grok or human creates WO with id `WO-2026-NNN` (plugins) or `WO-SAAS-NNN` (SaaS).
2. **Dispatch:** hermes-lead assigns seat(s); run `hermes_tool.py ops checklist`.
3. **Implement:** branch `cursor/<desc>-<suffix>` when Cloud; PR title **must** include WO id.
4. **Verify:** seat-specific SOP (DEV-001, SAAS-001, etc.) + evidence artifacts.
5. **Review:** human merge; agents do not force-push prod.
6. **Done:** human marks Airtable Done after PR link + evidence — ops seat does not write Airtable autonomously.

## WIP policy

- Max **2** active Cursor **JUCE** implementation WOs.
- One product lane per PR (no SaaS + plugin mixed).

## Verification

```bash
python3 disklordz/hermes/scripts/hermes_tool.py ops validate-pr --title "WO-2026-010: …"
```

## Related

- [GROK_CLOSED_LOOP_ENGINE.md](../../../docs/GROK_CLOSED_LOOP_ENGINE.md)
- [disklordz/automation/README.md](../../automation/README.md)
