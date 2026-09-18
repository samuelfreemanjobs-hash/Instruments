# Deploy Disklordz Drum SaaS (Vercel)

1. Import repo in Vercel; set **Root Directory** to `disklordz/website`.
2. Environment variables — **step-by-step:** [docs/VERCEL_ENV_AND_MCP.md](docs/VERCEL_ENV_AND_MCP.md) (UI table, redeploy, MCP, CLI).
   - `NEXT_PUBLIC_SUPABASE_URL`, `NEXT_PUBLIC_SUPABASE_ANON_KEY`
   - `SUPABASE_SERVICE_ROLE_KEY` — **required for production** (kits + credits)
   - `STRIPE_SECRET_KEY`, `STRIPE_WEBHOOK_SECRET`, `STRIPE_PRO_PRICE_ID` (Pro upgrades)
   - `SAAS_DAILY_GEN_LIMIT` (optional, default 20)
3. Supabase → Authentication → URL configuration → add  
   `https://YOUR_DOMAIN/auth/callback`
4. Run SQL migrations (SQL editor or CLI) **in order**:
   - `supabase/migrations/20260917190000_saved_kits.sql`
   - `supabase/migrations/20260917210000_kits_storage_bucket.sql`
   - `supabase/migrations/20260917220000_credits_billing.sql`
   - `supabase/migrations/20260917230000_saved_kits_history.sql`
   - `supabase/migrations/20260917240000_credits_variable_amount.sql`
5. Stripe: see [docs/STRIPE.md](docs/STRIPE.md).
6. Deploy. Full checklist: [docs/DISKLORDZ_GO_LIVE.md](../../docs/DISKLORDZ_GO_LIVE.md).
7. Automated smoke: `DISKLORDZ_URL=https://YOUR_DOMAIN npm run verify:go-live`

## Kit storage (WO-SAAS-009)

| Config | Behavior |
|--------|----------|
| Service role key set | WAVs uploaded to Supabase Storage bucket `disklordz-kits`; preview/download work across serverless instances |
| Local dev without service key | Falls back to OS temp only (same machine/session) |

Never expose `SUPABASE_SERVICE_ROLE_KEY` to the client or `NEXT_PUBLIC_*` vars.
