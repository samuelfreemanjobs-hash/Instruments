# VST Plugin Factory — inbox instructions

**To agent:** `to_agent` field in JSON (`cursor-vst-plugin-factory`, `night-circuit-qa`, `night-circuit-sound-design`, **`vst-gui-designer`**)  
**From:** `pm-agent` or `workflow-automation-agent` (scheduled jobs use `schedule_to_agent`)

1. Open handoffs where `"status": "open"` and `to_agent` matches your role (or newest factory handoff if you are **cursor-vst-plugin-factory**).
2. Follow role doc: Factory → [VST_PLUGIN_FACTORY_AGENT.md](../../docs/VST_PLUGIN_FACTORY_AGENT.md); GUI → [VST_GUI_DESIGNER_AGENT.md](../../docs/VST_GUI_DESIGNER_AGENT.md).
3. Create or use branch from `work_order.branch`.
4. Implement acceptance criteria; run `python3 vst-testing-ops/run_business.py --profile ci` before push.
5. Draft PR; title must include `work_order.id`.
6. **Required for scheduled WOs:** write `outbox/HO-<handoff_id>-done.json` per [docs/AUTOMATED_WORK_COMPLETION.md](../../docs/AUTOMATED_WORK_COMPLETION.md).

Bridge CLI: [scripts/vst-factory-bridge/vst-factory-bridge.sh](../../scripts/vst-factory-bridge/vst-factory-bridge.sh)

When a handoff lands on `main`, Slack may notify the team ([docs/VST_PLUGIN_FACTORY_SLACK.md](../../docs/VST_PLUGIN_FACTORY_SLACK.md)) — start **cursor-vst-plugin-factory** on the handoff branch.
