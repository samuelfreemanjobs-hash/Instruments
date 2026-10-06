# SOP-005 — Go-live and conversion smoke

## Purpose

**Conversion QA** path: prove generate → preview → factory → integrations after deploy or locally.

## Local

```bash
cd disklordz/website && npm run dev   # or existing tmux session on :3000
DISKLORDZ_URL=http://127.0.0.1:3000 bash scripts/verify-go-live.sh
```

## Production

See [DISKLORDZ_GO_LIVE.md](../../DISKLORDZ_GO_LIVE.md) and GitHub workflow `disklordz-go-live.yml` (workflow_dispatch, secrets in `docs/DISKLORDZ_GO_LIVE_SECRETS.md`).

## Checks included (automated script)

- `GET /` → 200
- `POST /api/generate` one_shot with variations
- Loop preview URL → 200
- `POST /api/factory/batch`
- `/api/integrations/status` → 200

## On failure

Use [incident template](../templates/TEMPLATE-incident-debug.md). Read `vst-testing-ops/error_log.txt` only for **plugin** lane failures.

## Agent

Primary owner: **conversion-qa** — see `disklordz/agents/profit/conversion-qa/agent.md`.
