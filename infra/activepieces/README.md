# Self-hosted Activepieces (Disklordz)

**Role:** Zapier replacement for **webhooks → Airtable → Slack** (and later Gumroad).  
**Not a replacement for:** Cursor **Airtable MCP** (PM Agent) or `tools/render_kit.py` (factory).

| Tool | Use |
|------|-----|
| **Cursor MCP** | `PM BRIEFING`, `PM UPDATE`, day-to-day registry |
| **Activepieces** | Event-driven glue when factory/store fires webhooks |
| **GitHub Action** | `sync_disklordz_pm_to_airtable.py` bootstrap |

## Prerequisites

- Docker + Docker Compose v2
- ~2 GB RAM free (Postgres + Redis + app + worker)
- Ports **8080** free on the host

## Quick start (studio machine)

Run on a machine with **Docker** (not the Cursor Cloud Agent VM — it has no Docker).

### Fully automated (recommended)

1. Copy env and set admin credentials (used once to create the account + factory flow):

```bash
cd infra/activepieces
cp .env.example .env
# edit: DISKLORDZ_AP_ADMIN_EMAIL, DISKLORDZ_AP_ADMIN_PASSWORD
# optional: DISKLORDZ_SLACK_WEBHOOK
```

2. One command — Docker up + provision webhook flow + test ping:

```bash
chmod +x automate.sh bootstrap.sh
./automate.sh
```

Writes **`infra/activepieces/.disklordz-webhook-url`**; `tools/render_kit.py` reads it automatically.

### Docker only

```bash
chmod +x bootstrap.sh
./bootstrap.sh
```

If `DISKLORDZ_AP_ADMIN_*` is already in `.env`, bootstrap also runs the provisioner after `docker compose up`.

**Manual** (same result):

```bash
cd infra/activepieces
cp .env.example .env
# openssl rand -hex 16  → AP_ENCRYPTION_KEY (32 hex chars)
# openssl rand -hex 32  → AP_JWT_SECRET, AP_POSTGRES_PASSWORD, AP_API_KEY
docker compose up -d
```

Open **http://localhost:8080** → create admin account → sign in.

### Remote access (optional)

Keep `AP_FRONTEND_URL` in `.env` equal to the URL you actually use in the browser (e.g. `https://pieces.yourdomain.com`), then put TLS in front:

| Method | Notes |
|--------|--------|
| **Tailscale** | Serve `8080` on the tailnet only — simplest for a solo studio |
| **Cloudflare Tunnel** | Public HTTPS without opening ports |
| **Caddy / nginx** | Reverse proxy to `localhost:8080` |

Do not expose plain `8080` on the public internet.

## Disklordz flows to build (in UI)

### 1. Factory batch complete

**Trigger:** Webhook (copy URL from flow)  
**Steps:** Slack → “Kit {body.kit_name} — {body.count} WAVs”  
**Optional:** HTTP or Airtable → append row / update project Notes

From factory after render:

```bash
curl -X POST "$ACTIVEPIECES_FACTORY_WEBHOOK_URL" \
  -H "Content-Type: application/json" \
  -d '{"kit_name":"GRAVEYARD_SHIFT","count":26,"repo":"Instruments"}'
```

Set the webhook URL for the factory (same value Activepieces shows on the Webhook trigger):

```bash
export DISKLORDZ_ACTIVEPIECES_WEBHOOK="https://your-host/api/v1/webhooks/..."
# or pass: python3 tools/render_kit.py ... --activepieces-webhook "$DISKLORDZ_ACTIVEPIECES_WEBHOOK"
```

### 2. Airtable — mirror PM sync (optional)

**Trigger:** Schedule (daily) or manual  
**Steps:** HTTP Request → `POST` your GitHub `workflow_dispatch` for [Sync Disklordz PM to Airtable](../../.github/workflows/sync-disklordz-pm.yml)  
*(Only if `AIRTABLE_API_KEY` is in GitHub secrets.)*

Or use Activepieces **Airtable** piece: update **Projects** row for factory `% Complete` when you prefer no-code over `PM UPDATE` in chat.

### 3. Store sale (later)

Webhook from Gumroad/Stripe → Airtable `product_ledger.sales_count` → Slack if near `max_supply`.

## Connect Airtable in Activepieces

1. **My connections** → **Airtable** → OAuth or PAT (same base `appUuhVQHAOv31wJ1`).
2. Test with **List records** on **Projects**.

## Ops

```bash
docker compose logs -f app
docker compose down          # stop
docker compose down -v       # stop + wipe DB (destructive)
```

Back up: volume `postgres_data` before upgrades.

## Security

- Keep `.env` local only (gitignored).
- Do not expose `8080` to the public internet without TLS + auth (use Tailscale, Cloudflare Tunnel, or reverse proxy).
- `AP_EXECUTION_MODE=UNSANDBOXED` is fine on a **private** studio machine; use sandboxed mode if multi-tenant.

## Version bumps

Change image tag in `docker-compose.yml` (e.g. `0.91.0` → latest [releases](https://github.com/activepieces/activepieces/releases)), then:

```bash
docker compose pull && docker compose up -d
```

## GitHub Actions (cloud factory)

Workflow **Disklordz factory render** (`.github/workflows/disklordz-factory-render.yml`) runs `render_kit.py` on `workflow_dispatch` and uploads WAV artifacts.

Repository secrets (optional):

| Secret | Purpose |
|--------|---------|
| `DISKLORDZ_ACTIVEPIECES_WEBHOOK` | Same URL as `.disklordz-webhook-url` — notifies flow after CI render |
| `DISKLORDZ_SLACK_WEBHOOK` | Direct Slack ping from `render_kit.py` (in addition to Activepieces) |

## Related docs

- [CURSOR_MCP_SETUP.md](../../docs/pm-agent/CURSOR_MCP_SETUP.md)
- [DISKLORDZ_FACTORY_PLAN.md](../../docs/DISKLORDZ_FACTORY_PLAN.md)
