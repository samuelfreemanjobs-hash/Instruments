# Deploy Disklordz Drum SaaS (Vercel)

1. Import repo in Vercel; set **Root Directory** to `disklordz/website`.
2. Environment variables:
   - `NEXT_PUBLIC_SUPABASE_URL`
   - `NEXT_PUBLIC_SUPABASE_ANON_KEY`
   - `SUPABASE_SERVICE_ROLE_KEY` — **required for production** (persistent kit audio on Vercel)
   - `SAAS_DAILY_GEN_LIMIT` (optional, default 20)
3. Supabase → Authentication → URL configuration → add  
   `https://YOUR_DOMAIN/auth/callback`
4. Run SQL migrations (SQL editor or CLI):
   - `supabase/migrations/20260917190000_saved_kits.sql`
   - `supabase/migrations/20260917210000_kits_storage_bucket.sql`
5. Deploy. Smoke test: generate → play all variations → download ZIP (can be minutes later if storage is on).

## Kit storage (WO-SAAS-009)

| Config | Behavior |
|--------|----------|
| Service role key set | WAVs uploaded to Supabase Storage bucket `disklordz-kits`; preview/download work across serverless instances |
| Local dev without service key | Falls back to OS temp only (same machine/session) |

Never expose `SUPABASE_SERVICE_ROLE_KEY` to the client or `NEXT_PUBLIC_*` vars.
