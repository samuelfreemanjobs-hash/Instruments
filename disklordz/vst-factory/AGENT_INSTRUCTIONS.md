# VST Plugin Factory — inbox instructions

**To agent:** `cursor-vst-plugin-factory`  
**From:** `pm-agent` or `workflow-automation-agent`

1. Open the newest `inbox/HO-*.json` with `"status": "open"`.
2. Follow [docs/VST_PLUGIN_FACTORY_AGENT.md](../../docs/VST_PLUGIN_FACTORY_AGENT.md).
3. Create or use branch from `work_order.branch`.
4. Implement acceptance criteria; run `python3 vst-testing-ops/run_business.py --profile ci` before push.
5. Draft PR; title must include `work_order.id`.
6. Optional: write completion summary to `outbox/HO-<same-id>-done.json` (mirror schema, `"status": "done"`).

Bridge CLI: [scripts/vst-factory-bridge/vst-factory-bridge.sh](../../scripts/vst-factory-bridge/vst-factory-bridge.sh)
