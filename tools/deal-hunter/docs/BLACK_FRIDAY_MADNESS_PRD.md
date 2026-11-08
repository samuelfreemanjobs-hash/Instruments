# Black Friday Madness — PRD implementation map

**Product:** AI-powered year-round shopping intelligence (peak events: Black Friday, Cyber Monday).

**Repo path:** `tools/deal-hunter/` (incremental toward full PRD; Next.js + Supabase migration tracked below).

| PRD nav (Part 3) | Current route / tab | Status |
|------------------|---------------------|--------|
| Dashboard | `hq` (HqDashboard) | Live |
| Discover Deals | `discover` | Live (filters: extend) |
| Product Search | `search` | Live |
| Categories | `search` + `/api/categories` | Partial |
| Price Tracker | `alerts` + watchlist | Partial |
| AI Shopping Assistant | `assistant` | Live |
| Coupons | `coupons` | Stub |
| Comparison Center | `pricing` + `compare` | Partial |
| Wishlist | `list` | Live (shared list) |
| Shopping Budget | `budget` | Stub |
| Deal Calendar | `calendar` | Stub |
| Community | `discover` community section | Live |
| Notifications | `alerts` + n8n | Partial |
| Account Settings | `settings` | Stub |

| PRD stack | Current | Target |
|-----------|---------|--------|
| Frontend | Vite + React PWA | Next.js App Router (Phase 2) |
| DB | SQLite demo | PostgreSQL + Supabase (see `supabase/migrations/`) |
| Auth | Local-first | Supabase Auth (Phase 2) |
| Jobs | n8n webhooks | Inngest/Trigger.dev + n8n glue |
| Hosting | Vercel static + Node API | Vercel full stack |

**Non‑negotiables (from PRD):**

- Authorized retailer data only (`server/integrations/data-sources.ts`).
- Explainable **Deal Intelligence Score** 0–100 (`server/intelligence/deal-score.ts`).
- Agents cannot purchase or hold payment credentials (`server/agents/orchestrator.ts`).
- Community deals labeled separately from verified offers.

**Admin (Part 3):** not in PWA v1 — planned Supabase-backed admin app.
