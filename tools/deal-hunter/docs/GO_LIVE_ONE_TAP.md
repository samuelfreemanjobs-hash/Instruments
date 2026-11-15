# Go live without a laptop

1. **GitHub secrets (one time):** `SUPABASE_ACCESS_TOKEN` + `SUPABASE_PROJECT_REF` (same as Disklordz is fine if one project). See [docs/BFM_GO_LIVE_SECRETS.md](../../../docs/BFM_GO_LIVE_SECRETS.md).

2. **Actions → Black Friday Madness go-live → Run workflow** (phone OK).

3. **Cursor → Republish** when an agent proposes catalog/auth changes.

After step 2, the workflow migrates DB, sets auth URLs, **syncs catalog**, and smoke-tests the live URL. Schedule runs twice daily for refresh.

Published app: with Supabase URL + anon key bound, **live catalog mode turns on automatically** unless `USE_SUPABASE_CATALOG=0`.
