# Savvy HQ deployment

## Vercel (PWA frontend)

1. Set project root to `tools/deal-hunter`.
2. Build: `npm run build` → `dist/`.
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

Cron example: POST `https://api.example.com/api/autopilot/run` every 15 minutes.

## Data compliance

Use only `server/integrations/data-sources.ts` approved connectors — affiliate feeds, PA-API, partner files. Do not scrape retailers without permission.

## Notifications

- **Web push**: client POST `/api/notifications/subscribe`; n8n triggers `/api/notifications/alert`.
- **Email**: route through n8n → SendGrid/Resend.
