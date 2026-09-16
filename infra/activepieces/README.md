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

## Quick start

```bash
cd infra/activepieces
cp .env.example .env
```

Generate secrets (run three times, paste into `.env`):

```bash
openssl rand -hex 16   # AP_ENCRYPTION_KEY (32 hex chars)
openssl rand -hex 16   # AP_JWT_SECRET
openssl rand -hex 32   # AP_API_KEY (optional in CE; set anyway)
```

Set `AP_POSTGRES_PASSWORD` to a strong password.

```bash
docker compose up -d
```

Open **http://localhost:8080** → create admin account → sign in.

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

(Add `ACTIVEPIECES_FACTORY_WEBHOOK_URL` to your shell or `tools/.env` locally.)

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

## Related docs

- [CURSOR_MCP_SETUP.md](../../docs/pm-agent/CURSOR_MCP_SETUP.md)
- [DISKLORDZ_FACTORY_PLAN.md](../../docs/DISKLORDZ_FACTORY_PLAN.md)
