# Black Friday Madness — Next.js (Phase 2–4)

## UI

- **shadcn-style** primitives under `src/components/ui/` (Button, Card, Input, Skeleton, EmptyState).
- **Ported flows:** Search, Discover, Assistant (client components calling Express via `/api` rewrite).
- **Phase 3:** `/product/[id]` (PRD detail + Recharts history), `/intel`, `/autopilot`, `/alerts` (Supabase `price_alerts` actions), `DealCard` links to product intel.
- **Phase 4:** `/coupons`, `/budget`, `/calendar`; API `USE_SUPABASE_CATALOG` read path; scheduled sync cron; steal alerts via `/api/notifications/steal-hits` + dispatch.

## Wishlist

Server Actions in `src/app/wishlist/actions.ts` — requires Supabase Auth + migration `002`.

## Sync catalog → Supabase

From repo root (`tools/deal-hunter`):

```bash
export SUPABASE_URL=...
export SUPABASE_SERVICE_ROLE_KEY=...
npm run sync:supabase
# or POST /api/admin/sync-catalog with header x-sync-secret (SYNC_CATALOG_SECRET)
```

Apply migration `003_sync_constraints.sql` before first sync.
