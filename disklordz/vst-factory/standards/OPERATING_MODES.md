# VST Factory — operating modes & approval gates

## Autonomy modes

Set on project create (`autonomy_mode` in factory-spec):

| Mode | Behavior |
|------|----------|
| `collaborative` (default) | All human gates required; Slack notifies when webhook set |
| `assisted` | Same gates; Cursor prompts auto-exported after scaffold |
| `autonomous` | Still requires architecture + release gates; GUI gate may be pre-approved by director policy only when explicitly recorded |

Webhook is **never** required. Missing `VST_FACTORY_SLACK_WEBHOOK_URL` → silent no-op notify.

## Approval gates

| Gate | Created when | Required before |
|------|--------------|-----------------|
| `approve_architecture` | Project create | `POST .../scaffold` |
| `approve_gui` | Design phase verified via sync | Accepting impl completion (`sync` → `impl`) |
| `approve_release` | Certify passed | `POST .../ship` |
| `approve_dsp_promotion` | DSP research start | Promoting research → verified knowledge |

Resolve via `POST /api/projects/{id}/approvals/{approval_id}/resolve` with `{"status":"approved"}` or `rejected`.

## Pipeline

Data-driven stages live in `modules/workflows/full-lifecycle.json`. Inspect with `GET /api/projects/{id}/pipeline`. Sync APC progress with `POST /api/projects/{id}/sync`.
