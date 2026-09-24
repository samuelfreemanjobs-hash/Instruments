# VST Plugin Factory — automation (PM + Workflow Automation Agent)

Hand off Airtable **`[Plugin][JUCE]`** work orders to **cursor-vst-plugin-factory** via git inbox files (same pattern as Antigravity).

## Local / Action handoff

```bash
cd disklordz/automation
cp .env.example .env   # AIRTABLE_* when using --patch-airtable
python3 scripts/wo_to_vst_factory_handoff.py --work-order-id WO-2026-JUCE-001 --branch cursor/my-feature-ddc8
```

Creates `disklordz/vst-factory/inbox/HO-*.json`. Commit + push, or use GitHub Action below.

## Airtable button → GitHub

Automation → Run script or Zapier → `POST` **repository_dispatch**:

```http
POST https://api.github.com/repos/samuelfreemanjobs-hash/Instruments/dispatches
Authorization: Bearer <GITHUB_PAT with repo scope>
Accept: application/vnd.github+json

{
  "event_type": "airtable-vst-factory-handoff",
  "client_payload": {
    "work_order_id": "WO-2026-JUCE-001",
    "branch": "cursor/my-feature-ddc8"
  }
}
```

Workflow: [`.github/workflows/airtable-vst-factory-handoff.yml`](../../.github/workflows/airtable-vst-factory-handoff.yml)

## Scheduled automation (Workflow Automation Agent)

Workflow: [`.github/workflows/vst-plugin-factory-schedule.yml`](../../.github/workflows/vst-plugin-factory-schedule.yml)

| UTC | Job | Purpose |
|-----|-----|---------|
| Mon 09:00 | `plugin-ci-health` | `run_business.py --profile ci` |
| Wed 10:00 | `night-circuit-integrity` | Night Circuit VST3 + `NightCircuitTests` + pluginval (VM — not FL/MPC) |
| Fri 17:00 | `doc-sync` | ARCHITECTURE / GUI spec checklist |

**Cursor Automations** (Mon triage, QA Gate 1, Fri docs, biweekly sound-design): full calendar in [docs/VST_PLUGIN_FACTORY_SCHEDULE.md](../../docs/VST_PLUGIN_FACTORY_SCHEDULE.md).

Manual re-run: Actions → **VST Plugin Factory schedule** → `workflow_dispatch` → pick job.

## Slack (Plugin Factory agent)

```bash
./scripts/setup-disklordz-integrations.sh slack-vst-factory \
  --webhook-url 'https://hooks.slack.com/services/...' \
  --mention-user-id U0XXXXXXXX
```

Posts: new **inbox** handoffs on `main`, **schedule** workflow results. Full guide: [docs/VST_PLUGIN_FACTORY_SLACK.md](../../docs/VST_PLUGIN_FACTORY_SLACK.md).

## Manual bridge (no Airtable)

```bash
chmod +x scripts/vst-factory-bridge/vst-factory-bridge.sh
./scripts/vst-factory-bridge/vst-factory-bridge.sh send \
  --wo WO-LOCAL-001 \
  --title "[Plugin][JUCE] …" \
  --branch cursor/feature-ddc8 \
  --push
```

## Cursor Cloud Agent

Start a Cloud Agent with the prompt template in [docs/VST_PLUGIN_FACTORY_AGENT.md](../../docs/VST_PLUGIN_FACTORY_AGENT.md), or wire Cursor Automations to cron entries in [docs/VST_PLUGIN_FACTORY_SCHEDULE.md](../../docs/VST_PLUGIN_FACTORY_SCHEDULE.md).
