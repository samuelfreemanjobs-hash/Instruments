# Cursor Automations — Plugin Factory & Night Circuit

Repo-managed **prompts** live under `docs/*_SUBAGENT.md` and `docs/VST_PLUGIN_FACTORY_AGENT.md`.  
**GitHub** emits scheduled **inbox handoffs** ([agent-scheduled-handoffs.yml](../../.github/workflows/agent-scheduled-handoffs.yml)).

Create Automations in the Cursor dashboard (repo **Instruments**, branch from handoff or `disklordz/automation/scheduled_work.yaml` `default_branch`).

| Automation name | Cron (UTC) | Agent | Trigger pairing |
|-----------------|------------|-------|-----------------|
| `vst-factory-monday-health-triage` | `30 9 * * 1` | cursor-vst-plugin-factory | After Mon health; or consume `WO-2026-JUCE-HEALTH-RECUR` handoff |
| `night-circuit-qa-gate1` | `0 10 * * 1` | night-circuit-qa | `WO-2026-NC-QA-GATE1-RECUR` handoff |
| `night-circuit-qa-bank` | `30 10 * * 3` | night-circuit-qa | After Wed integrity |
| `night-circuit-sound-design-batch` | `0 16 * * 4` | night-circuit-sound-design | `WO-2026-NC-DESIGN-BIWEEK-RECUR` (skip even ISO weeks optional) |
| `vst-factory-friday-doc-sync` | `30 17 * * 5` | cursor-vst-plugin-factory | `WO-2026-JUCE-DOCSYNC-RECUR` |
| `vst-factory-inbox-consumer` | On push / daily | cursor-vst-plugin-factory | Newest open `inbox/HO-*.json` |
| `vst-gui-designer-weekly` | `25 17 * * 5` | **vst-gui-designer** | `WO-2026-JUCE-GUI-AUDIT-RECUR` handoff · [VST_GUI_DESIGNER_AGENT.md](../../docs/VST_GUI_DESIGNER_AGENT.md) |

Prompt bodies: [VST_PLUGIN_FACTORY_SCHEDULE.md](../../docs/VST_PLUGIN_FACTORY_SCHEDULE.md) § Cursor Automation catalog.

Completion rules: [AUTOMATED_WORK_COMPLETION.md](../../docs/AUTOMATED_WORK_COMPLETION.md).
