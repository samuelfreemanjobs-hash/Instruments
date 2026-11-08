# Publish on Cursor Apps

Live app: **https://black-friday-madness.apps.grok.bot** (private by default).

## Bundled demo (no secrets)

With **no** `DEAL_API_ORIGIN`, the published build serves the **bundled demo catalog** (search, product intel, compare, assistant stubs). No external API required.

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
