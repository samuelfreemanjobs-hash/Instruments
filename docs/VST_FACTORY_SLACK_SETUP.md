# VST Factory — Slack approval notifications

Pending approval gates (`approve_architecture`, `approve_dsp_promotion`, `approve_gui`, `approve_release`) can post to Slack when the factory orchestrator creates or updates a gate.

## Channel (default)

| Slack | Value |
|-------|--------|
| Name | `#disklordz-factory-dept` |
| Channel ID | `C0CG2AQ8SJ0` |

Override with env `VST_FACTORY_SLACK_CHANNEL_ID`.

## One-time setup (your machine / worker)

1. In Slack: **Apps → Incoming Webhooks** (or create a Slack app with Incoming Webhooks).
2. Add webhook to `#disklordz-factory-dept`.
3. Export on the host that runs `uvicorn` (never commit the URL):

```bash
export VST_FACTORY_SLACK_WEBHOOK_URL='https://hooks.slack.com/services/...'
export VST_FACTORY_SLACK_CHANNEL_ID='C0CG2AQ8SJ0'   # optional if webhook is channel-scoped
export VST_FACTORY_PUBLIC_URL='http://127.0.0.1:8790'  # or tunnel URL for phone
```

4. Restart factory: `uvicorn vst_factory.main:app --host 127.0.0.1 --port 8790`

5. Smoke test:

```bash
cd disklordz/vst-factory
python3 -c "
from vst_factory.slack_notify import notify_gate
print(notify_gate(plugin_name='MeterTap', project_id=0, gate='setup_test'))
"
```

## Approve from factory UI

When the webhook is set, create a project in the UI; Slack should show `approve_architecture` pending. Use **Approve architecture** (or API below) before **Scaffold**.

## API (approve)

```http
GET /api/projects/{id}/approvals
POST /api/projects/{id}/approvals/{approval_id}/resolve
Content-Type: application/json

{"status": "approved"}
```

## Cursor / phone

- GitHub mobile for CI; Slack for **gates**.
- Approve-from-Slack only (reply APPROVE) is **V2** — notifications first.

## Related

- [OPERATING_MODES.md](../disklordz/vst-factory/standards/OPERATING_MODES.md)
- [DISKLORDZ_VST_FACTORY_PLATFORM.md](DISKLORDZ_VST_FACTORY_PLATFORM.md)
