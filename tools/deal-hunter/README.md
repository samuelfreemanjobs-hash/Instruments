# Black Friday Madness

Year-round AI shopping intelligence (Black Friday / Cyber Monday as peak events). **Authorized retailer data only**, explainable Deal Intelligence Score 0–100, Supabase auth for wishlists and alerts.

## Quick start (Next.js — default)

```bash
npm ci && npm install --prefix web
cp .env.example .env   # API + optional Supabase/VAPID/Inngest
cp .env.example web/.env.local   # NEXT_PUBLIC_SUPABASE_* , DEAL_API_ORIGIN
npm run dev
```

Open **http://127.0.0.1:3000** — UI proxies `/api/*` to Express on **:3847**.

## Legacy Vite PWA

See [LEGACY_PWA.md](./LEGACY_PWA.md) — `npm run dev:legacy` (:5173) for regression only.

## Docs

- [ARCHITECTURE.md](./ARCHITECTURE.md)
- [docs/BLACK_FRIDAY_MADNESS_PRD.md](./docs/BLACK_FRIDAY_MADNESS_PRD.md)
- [DEPLOY.md](./DEPLOY.md)
- [web/README.md](./web/README.md)

## Verify

```bash
npm test && npm run build
```
