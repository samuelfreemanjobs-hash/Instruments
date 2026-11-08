# Black Friday Madness — architecture

**Product:** AI-powered year-round shopping intelligence (`tools/deal-hunter/`).

## Build & run

```bash
cd tools/deal-hunter
npm ci && npm install --prefix web
npm run dev          # Express :3847 + Next.js :3000
npm test && npm run build
```

- **Phase 2 UI:** `web/` — Next.js 15 App Router, Tailwind 4, `@supabase/ssr`.
- **Legacy PWA:** `npm run dev:legacy` (Vite on :5173).
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
