# Black Friday Madness deployment

## Vercel (production — Next.js)

1. Set **Root Directory** to `tools/deal-hunter/web`.
2. Build: `npm run build` (Next 15).
3. Env: `NEXT_PUBLIC_SUPABASE_*`, `DEAL_API_ORIGIN` → production API URL, `NEXT_PUBLIC_USE_SUPABASE_CATALOG=1` after sync.
4. Inngest: `INNGEST_EVENT_KEY`, `INNGEST_SIGNING_KEY` — sync app URL `https://<domain>/api/inngest`.
5. Web Push: `NEXT_PUBLIC_VAPID_PUBLIC_KEY` + API host `VAPID_*` keys.
6. Admin: `BFM_ADMIN_EMAILS`, `BFM_ADMIN_SECRET` (matches API `x-admin-secret`).
7. Supabase redirect: `https://<your-domain>/auth/callback`.

## Vercel (legacy PWA)

1. Set project root to `tools/deal-hunter`.
2. Build: `npm run build:legacy` → `dist/`.
3. Replace `YOUR_DEAL_API_HOST` in `vercel.json` with your Node API host (Fly, Railway, VPS, etc.).
4. Env: `VITE_API_PROXY` is dev-only; production uses same-origin rewrites or explicit API URL.

SQLite (`better-sqlite3`) does **not** run on Vercel serverless — host the Express API on a long-lived Node service.

## API host

```bash
npm run build && NODE_ENV=production PORT=3847 DEAL_HUNTER_DB=/data/deal-hunter.sqlite npm start
```

## n8n automation

Import `integrations/n8n/savvy-hq-autopilot-stub.json`.

| Webhook env | Purpose |
|-------------|---------|
| `N8N_WEBHOOK_URL` / `N8N_WEBHOOK_DEAL_ALERT` | Steal / target alerts → email + web push relay |
| `N8N_WEBHOOK_EMAIL` | Digest emails |
| `N8N_WEBHOOK_SECRET` | Header `x-n8n-secret` on `/api/autopilot/run` and `/api/notifications/alert` |

Cron examples (header `x-n8n-secret: $N8N_WEBHOOK_SECRET`):

- POST `https://api.example.com/api/autopilot/run` every 15 minutes.
- POST `https://api.example.com/api/admin/sync-catalog/cron` hourly (or set `CATALOG_SYNC_INTERVAL_MINUTES=60` on the API host).
- POST `https://api.example.com/api/notifications/dispatch-steals` every 5–15 minutes for steal push/email relay.

Set `USE_SUPABASE_CATALOG=1` on the API so search/discover read synced Postgres prices with SQLite fallback.

## Data compliance

Use only `server/integrations/data-sources.ts` approved connectors — affiliate feeds, PA-API, partner files. Do not scrape retailers without permission.

## Notifications

- **Web push**: client POST `/api/notifications/subscribe`; n8n triggers `/api/notifications/alert`.
- **Email**: route through n8n → SendGrid/Resend.
