# DiskLordz VST Factory

See [ARCHITECTURE.md](ARCHITECTURE.md) and [docs/DISKLORDZ_VST_FACTORY.md](../../docs/DISKLORDZ_VST_FACTORY.md).

## Install & run

```bash
cd disklordz/vst-factory
python3 -m venv .venv && source .venv/bin/activate
pip install -e ".[dev]"
pytest -q
uvicorn vst_factory.main:app --host 127.0.0.1 --port 8790
```

### PowerShell

```powershell
cd disklordz\vst-factory
py -3 -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install -e ".[dev]"
pytest -q
$env:VST_FACTORY_DB = "$env:TEMP\factory_projects.sqlite"
$env:VST_FACTORY_SLACK_WEBHOOK_URL = ""   # leave empty — Slack no-op
$env:VST_FACTORY_PUBLIC_URL = "http://127.0.0.1:8790"
$env:VST_FACTORY_URL = "http://127.0.0.1:8790"
uvicorn vst_factory.main:app --host 127.0.0.1 --port 8790
```

### Bash env vars

```bash
export VST_FACTORY_DB=factory_projects.sqlite
export VST_FACTORY_SLACK_WEBHOOK_URL=     # unset/empty → no Slack posts
export VST_FACTORY_PUBLIC_URL=http://127.0.0.1:8790
export VST_FACTORY_URL=http://127.0.0.1:8790
```

Never commit a Slack webhook URL. Setup: [docs/VST_FACTORY_SLACK_SETUP.md](../../docs/VST_FACTORY_SLACK_SETUP.md).

## Lifecycle API (quick)

| Step | Call |
|------|------|
| Sync APC | `POST /api/projects/{id}/sync` |
| Pipeline | `GET /api/projects/{id}/pipeline` |
| Scaffold | `POST /api/projects/{id}/scaffold` (needs `approve_architecture`) |
| Ship | `POST /api/projects/{id}/ship` (needs certify + `approve_release`) |

Workflow definition: [`modules/workflows/full-lifecycle.json`](modules/workflows/full-lifecycle.json).

## MeterTap smoke

With the factory already listening on port 8790:

```bash
python scripts/complete_metertap.py
# or
bash scripts/smoke_metertap.sh
```

Exercises health → create → approve_architecture → **/sync** → **/pipeline** → portfolio/taxonomy.

Optional: `--through-scaffold` (writes `vst/` — only after Dream/Plan/Design).
