# Complete live checklist (no laptop)

1. **Cursor Publish Secrets:** `SUPABASE_URL`, `SUPABASE_ANON_KEY` (bound). Optional: `USE_SUPABASE_CATALOG=1` after sync (auto-on when only anon+url bound unless set to `0`).
2. **GitHub repo secrets:** `SUPABASE_ACCESS_TOKEN`, `SUPABASE_PROJECT_REF` → [BFM_GO_LIVE_SECRETS.md](../../../docs/BFM_GO_LIVE_SECRETS.md).
3. **Actions → Black Friday Madness go-live → Run workflow** (migrate, auth URLs, catalog sync, smoke test).
4. **Republish** after code changes only.

**Done when:** `https://black-friday-madness.apps.grok.bot/api/catalog/source` shows `useSupabase: true` and search returns rows; login + wishlist work.