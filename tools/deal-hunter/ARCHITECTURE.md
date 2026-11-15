# Black Friday Madness — architecture

**Product:** AI-powered year-round shopping intelligence (`tools/deal-hunter/`).

## Build & run

```bash
cd tools/deal-hunter
npm ci && npm install --prefix web
npm run dev          # Express :3847 + Next.js :3000
npm test && npm run build
```

- **Primary UI:** `web/` — Next.js 15 App Router, Tailwind 4, `@supabase/ssr` (production default on Vercel).
- **Legacy PWA:** [LEGACY_PWA.md](./LEGACY_PWA.md) — Vite `:5173` regression only.
- **Catalog pool:** `server/catalog-pool.ts` — SQLite drift + Supabase overlay (`USE_SUPABASE_CATALOG=1` on API).
- **Supabase search:** `web/src/app/api/catalog/search` when `NEXT_PUBLIC_USE_SUPABASE_CATALOG=1` (migration `006`).
- **Jobs:** Inngest crons in `web/src/inngest/` (+ n8n `x-n8n-secret` on API).
- **Web Push:** VAPID + `web-push` on API; `web/public/sw.js` + `NEXT_PUBLIC_VAPID_PUBLIC_KEY`.
- **Admin:** `/admin` — `BFM_ADMIN_EMAILS` + Server Actions → `GET /api/admin/status`.
- **Supabase:** `supabase/README.md` + migrations `001` + `002` (RLS).

## PRD alignment

See [docs/BLACK_FRIDAY_MADNESS_PRD.md](./docs/BLACK_FRIDAY_MADNESS_PRD.md) for screen-by-screen status.

| Layer | Module |
|-------|--------|
| Deal Intelligence Score 0–100 | `server/intelligence/deal-score.ts` |
| Analysis + briefing | `server/intelligence/engine.ts` |
| Chief agent + specialists | `server/agents/orchestrator.ts`, `autonomous-fleet.ts` |
| Retailer adapters (authorized) | `server/retailers/types.ts`, `demo-adapter.ts` |
| REST v1 | `server/api/v1/routes.ts` → `/api/v1/*` |
| Postgres schema (target) | `supabase/migrations/001_black_friday_madness.sql` |
| Demo persistence | SQLite `server/db.ts` |

## Principles

- No unrestricted scraping — `server/integrations/data-sources.ts`.
- Agents never purchase or hold payment credentials — `orchestrator.ts`.
- Community deals ≠ verified catalog offers — `server/community.ts`.

## Deploy

[Vercel static + Node API](./DEPLOY.md); n8n for external automation glue.
