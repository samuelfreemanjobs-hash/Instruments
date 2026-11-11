# Production deploy checklist — Black Friday Madness

Split stack: **Next.js on Vercel** + **Express API on Fly.io / Railway / VPS** + **Supabase Postgres**.

## 1. Supabase

- [ ] Project created; migrations `001`–`007` applied (`supabase db push`)
- [ ] Auth redirect URLs: `https://<vercel-domain>/auth/callback`, local `http://localhost:3000/auth/callback`
- [ ] RLS verified for `wishlists`, `price_alerts`, `user_push_subscriptions`

## 2. API host (long-lived Node)

| Variable | Notes |
|----------|--------|
| `PORT` | `3847` |
| `DEAL_HUNTER_DB` | Persistent volume path |
| `SUPABASE_URL` + `SUPABASE_SERVICE_ROLE_KEY` | Catalog sync + push target read |
| `USE_SUPABASE_CATALOG` | `1` after first sync |
| `SYNC_CATALOG_SECRET` | Protects `POST /api/admin/sync-catalog` |
| `N8N_WEBHOOK_SECRET` | Cron: sync, steals, autopilot |
| `VAPID_PUBLIC_KEY` / `VAPID_PRIVATE_KEY` | Web Push send |
| `BFM_ADMIN_SECRET` | `GET /api/admin/status` |

```bash
npm ci && npm run build:server && npm start
```

## 3. Vercel (root: `tools/deal-hunter/web`)

| Variable | Notes |
|----------|--------|
| `DEAL_API_ORIGIN` | `https://api.yourdomain.com` |
| `NEXT_PUBLIC_SUPABASE_*` | Anon key only |
| `NEXT_PUBLIC_USE_SUPABASE_CATALOG` | `1` when catalog synced |
| `NEXT_PUBLIC_VAPID_PUBLIC_KEY` | Same as API public VAPID |
| `INNGEST_EVENT_KEY` / `INNGEST_SIGNING_KEY` | `/api/inngest` sync URL |
| `BFM_ADMIN_EMAILS` | Comma list for `/admin` |
| `BFM_ADMIN_SECRET` | Same value as API |

## 4. Secret sync

- API and Vercel share: `BFM_ADMIN_SECRET`, `N8N_WEBHOOK_SECRET` (if Inngest calls API)
- VAPID: public → Next `NEXT_PUBLIC_*`; private → API only
- Never put `SUPABASE_SERVICE_ROLE_KEY` on Vercel

## 5. Post-deploy smoke

```bash
curl https://api.yourdomain.com/api/health
curl https://<vercel>/api/catalog/source  # via rewrite if configured
cd tools/deal-hunter/web && DEAL_API_ORIGIN=... npx playwright test
```

## 6. Jobs

- Inngest: register `https://<vercel>/api/inngest`
- Or n8n cron → `POST .../api/admin/sync-catalog/cron` and `.../dispatch-steals` with `x-n8n-secret`

## 7. Cursor Apps (optional)

See [docs/CURSOR_PUBLISH.md](./CURSOR_PUBLISH.md). Bind `DEAL_API_ORIGIN` to your public API after publish.
