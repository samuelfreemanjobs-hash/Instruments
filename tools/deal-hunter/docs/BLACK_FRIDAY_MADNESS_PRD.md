# Black Friday Madness — PRD implementation map

**Product:** AI-powered year-round shopping intelligence (peak events: Black Friday, Cyber Monday).

**Repo path:** `tools/deal-hunter/` (incremental toward full PRD; Next.js + Supabase migration tracked below).

| PRD nav (Part 3) | Current route / tab | Status |
|------------------|---------------------|--------|
| Dashboard | `hq` (HqDashboard) | Live |
| Discover Deals | `discover` | Live (filters: extend) |
| Product Search | `search` | Live |
| Categories | `/categories` + `/search` | Live |
| Price Tracker | `alerts` + watchlist | Partial |
| AI Shopping Assistant | `assistant` | Live |
| Coupons | `/coupons` (Next) | Live |
| Comparison Center | `/compare` (Next) | Live |
| Wishlist | `/wishlist` | Live (Supabase + Server Actions) |
| Shopping Budget | `/budget` | Live |
| Deal Calendar | `/calendar` | Live |
| Community | `discover` community section | Live |
| Notifications | `alerts` + n8n | Partial |
| Account Settings | `/settings` | Live (profile + notification prefs) |

| PRD stack | Current | Target |
|-----------|---------|--------|
| Frontend | Vite PWA (`npm run dev:legacy`) | **Next.js** in `web/` (`npm run dev`) |
| DB | SQLite demo API | PostgreSQL + Supabase (`supabase/migrations/`) |
| Auth | Local-first legacy | **Supabase Auth** magic link (`web/src/app/login`) |
| Jobs | n8n webhooks | Inngest/Trigger.dev + n8n glue |
| Hosting | Vercel static + Node API | Vercel full stack |

**Non‑negotiables (from PRD):**

- Authorized retailer data only (`server/integrations/data-sources.ts`).
- Explainable **Deal Intelligence Score** 0–100 (`server/intelligence/deal-score.ts`).
- Agents cannot purchase or hold payment credentials (`server/agents/orchestrator.ts`).
- Community deals labeled separately from verified offers.

**Admin (Part 3):** not in PWA v1 — planned Supabase-backed admin app.
