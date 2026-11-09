# SOP-VF-07 — Factory operations (server, Slack, gates)

## Purpose

Run the DiskLordz VST Factory service, keep Slack gate notifications working, and move products through approvals.

## Start the factory

**Linux / macOS**

```bash
cd disklordz/vst-factory
python3 -m venv .venv && source .venv/bin/activate && pip install -e ".[dev]"
export VST_FACTORY_SLACK_WEBHOOK_URL='https://hooks.slack.com/services/…'   # optional; never commit
uvicorn vst_factory.main:app --host 127.0.0.1 --port 8790
```

**Windows (PowerShell)**

```powershell
cd disklordz\vst-factory
py -3 -m venv .venv; .\.venv\Scripts\python.exe -m pip install -e ".[dev]"
$env:VST_FACTORY_SLACK_WEBHOOK_URL = '<webhook>'      # set in this window only
.\.venv\Scripts\uvicorn.exe vst_factory.main:app --host 127.0.0.1 --port 8790
```

UI: http://127.0.0.1:8790 · health: `GET /api/health`.

## Slack webhook (one-time)

1. [api.slack.com/apps](https://api.slack.com/apps) → your app (or Create New App → From scratch) → **Incoming Webhooks** → on → **Add New Webhook** → `#disklordz-factory-dept`.
2. Set `VST_FACTORY_SLACK_WEBHOOK_URL` in the shell that runs uvicorn. Never paste it in chat or commit it.
3. Test: `python -c "from vst_factory.slack_notify import notify_gate; print(notify_gate(plugin_name='Test', project_id=0, gate='setup_test'))"` → `{'sent': True, ...}`.

## Daily flow

| Step | API | Notes |
|------|-----|-------|
| Intake | `POST /api/projects` | prompt, class, name, category |
| Sync phase from repo | `POST /api/projects/{id}/sync` | reads `plugins/<Name>/status.json` + apc-gate |
| See pipeline | `GET /api/projects/{id}/pipeline` | stages, blocking gate, next SOP |
| Approve gate | `POST /api/projects/{id}/approvals/{aid}/resolve` `{"status":"approved"}` | or UI button |
| Scaffold | `POST /api/projects/{id}/scaffold` | needs `approve_architecture` |
| Build / test / certify | `POST …/build`, `…/test`, `…/certify` | runs repo scripts |
| Ship | `POST /api/projects/{id}/ship` | needs certify + `approve_release` |

## Gates

| Gate | Created when | Approve after reviewing |
|------|--------------|-------------------------|
| `approve_architecture` | intake | constitution, brief, parameter spec, architecture open issues |
| `approve_gui` | design verified | design-spec + theme preview |
| `approve_dsp_promotion` | DSP research promotes an algorithm | research notes + tests |
| `approve_release` | certify passed | release notes, verify evidence, pluginval |

## Smoke test

`python3 disklordz/vst-factory/scripts/complete_metertap.py` (server running) → `PASS MeterTap smoke`. Use a throwaway DB for smoke runs: `VST_FACTORY_DB=<temp>.sqlite`.

## Pitfalls

- The import graph must be complete on the branch you run: `python -c "import vst_factory.main"` before starting.
- Placeholder webhook values (`…/services/...`) fail every post; leave unset instead.
- Port 3000 is used by other agents; the factory uses 8790.

## Related

[disklordz/vst-factory/ARCHITECTURE.md](../../../disklordz/vst-factory/ARCHITECTURE.md) · [VST_FACTORY_SLACK_SETUP.md](../../VST_FACTORY_SLACK_SETUP.md)
