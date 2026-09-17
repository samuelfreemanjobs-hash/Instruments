# Deploy Disklordz Drum SaaS (Vercel)

1. Import repo in Vercel; set **Root Directory** to `disklordz/website`.
2. Environment variables:
   - `NEXT_PUBLIC_SUPABASE_URL`
   - `NEXT_PUBLIC_SUPABASE_ANON_KEY`
   - `SAAS_DAILY_GEN_LIMIT` (optional, default 20)
3. Supabase → Authentication → URL configuration → add  
   `https://YOUR_DOMAIN/auth/callback`
4. Run SQL migration: `supabase/migrations/20260917190000_saved_kits.sql`
5. Deploy. Smoke test: generate → play → download ZIP.

**Note:** Generated kits use `/tmp` on the server; on Vercel, download soon after generate (same region/instance) or rely on client-side preview URLs during the session.
