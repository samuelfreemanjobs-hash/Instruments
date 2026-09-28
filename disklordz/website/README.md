# Disklordz Drum SaaS (website)

Next.js app for Drum SaaS **v0 MVP** (parametric factory, preview, ZIP, auth, rate limits). See [DEPLOY.md](./DEPLOY.md).

```bash
npm install
npm run dev
```

Deploy: Vercel root directory `disklordz/website`.

Copy `.env.example` → `.env.local` and run the SQL migration in `supabase/migrations/` on your Supabase project.

| Variable | Default | Purpose |
|----------|---------|---------|
| `NEXT_PUBLIC_SUPABASE_URL` | — | Supabase project URL |
| `NEXT_PUBLIC_SUPABASE_ANON_KEY` | — | Anon key (browser-safe) |
| `SAAS_DAILY_GEN_LIMIT` | `20` | Free-tier gens per IP per day |

See [ARCHITECTURE.md](./ARCHITECTURE.md) and [docs/DISKLORDZ_SAAS_V0.md](../../docs/DISKLORDZ_SAAS_V0.md).
