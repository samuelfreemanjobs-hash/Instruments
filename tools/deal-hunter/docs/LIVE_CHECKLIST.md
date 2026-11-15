# Complete live checklist (no laptop)

## Option A — Cursor only (recommended)

1. **Publish Secrets:** `SUPABASE_URL`, `SUPABASE_ANON_KEY`, **`SUPABASE_SERVICE_ROLE_KEY`**, **`BFM_ADMIN_EMAILS`** (your login email), `USE_SUPABASE_CATALOG=1` (or leave unset for auto-on).
2. **Republish** the app.
3. Sign in → **`/admin`** → **Sync catalog → Supabase** (fills Postgres from bundled JSON).
4. Supabase dashboard → **Authentication → URL configuration:** site URL + `/auth/callback` for `https://black-friday-madness.apps.grok.bot`.

## Option B — GitHub Actions (migrations + full pipeline sync)

1. Add repo secrets `SUPABASE_ACCESS_TOKEN`, `SUPABASE_PROJECT_REF` → [BFM_GO_LIVE_SECRETS.md](../../../docs/BFM_GO_LIVE_SECRETS.md).
2. **Actions → Black Friday Madness go-live → Run workflow.**

**Done when:** `/api/catalog/search?q=tv` returns rows; wishlist works signed-in.