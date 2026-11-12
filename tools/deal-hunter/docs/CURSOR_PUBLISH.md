# Publish on Cursor Apps

Live app: **https://black-friday-madness.apps.grok.bot** (public).

## Bundled demo (no secrets)

With **no** `DEAL_API_ORIGIN`, the published build serves the **bundled demo catalog** (search, product intel, compare, assistant stubs). No external API required.

## Supabase (auth / wishlist)

1. Create project → run `npm run supabase:migrate` from `tools/deal-hunter`.
2. In Supabase **Authentication → URL Configuration** set **Site URL** to the live app origin and add Redirect URL  
   `https://black-friday-madness.apps.grok.bot/auth/callback`. Sign-in is **email + password** (Confirm email OFF — no OTP mail).
3. In Cursor app Secrets add: `SUPABASE_URL`, `SUPABASE_ANON_KEY`, **`USE_SUPABASE_CATALOG=1`** (required for live catalog search and product pages; without it the bundled demo JSON is used).
4. **Automated (no laptop):** add GitHub secrets in [docs/BFM_GO_LIVE_SECRETS.md](../../../docs/BFM_GO_LIVE_SECRETS.md), then **Actions → Black Friday Madness go-live → Run workflow**.
5. **Or local:** `npm run go-live` (needs `SUPABASE_ACCESS_TOKEN`, `SUPABASE_PROJECT_REF`, `SUPABASE_SERVICE_ROLE_KEY` in env).
6. Republish only if you changed Cursor secrets.

Local: `npm run setup:env` then fill `tools/deal-hunter/.env` once; the script mirrors into `web/.env.local`.

## Optional secrets (Cursor → app → Secrets)

Add these in the publish settings secure form (never in chat):

| Secret | Purpose |
|--------|---------|
| `SUPABASE_URL` | Auth, wishlist, alerts, push subs |
| `SUPABASE_ANON_KEY` | Browser auth (injected at runtime, not in source) |
| `USE_SUPABASE_CATALOG` | Set `1` after Supabase catalog sync |
| `DEAL_API_ORIGIN` | Public URL of the Express deal API (Fly/Railway). Omit for bundled demo. |
| `VAPID_PUBLIC_KEY` | Web push (matches API private VAPID) |
| `BFM_ADMIN_EMAILS` | Comma list for `/admin` |
| `BFM_ADMIN_SECRET` | Must match API |
| `INNGEST_EVENT_KEY` / `INNGEST_SIGNING_KEY` | Scheduled jobs |
| `N8N_WEBHOOK_SECRET` | Cron headers to API |

Local dev: `cd tools/deal-hunter && npm run dev` (:3000 + :3847).

## Site unreachable (ERR_CONNECTION_CLOSED / SSL)

If the URL resolves in DNS but TLS fails (browser “connection closed”, `openssl` shows **no peer certificate**), the **Cursor Apps host** for that slug is down or not serving a cert — not a Next.js bug. Recovery:

1. **Cursor → Publish** for `black-friday-madness` → **Republish** the latest `main` commit (redeploy).
2. If still broken after a successful publish notification, use **Cursor support** / Apps dashboard for the slug; repo changes cannot fix platform routing.
3. Catalog/sync work (GitHub go-live) only matters **after** the URL loads HTTPS.
