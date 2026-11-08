# Black Friday Madness — Next.js (Phase 2)

## UI

- **shadcn-style** primitives under `src/components/ui/` (Button, Card, Input, Skeleton, EmptyState).
- **Ported flows:** Search, Discover, Assistant (client components calling Express via `/api` rewrite).

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
