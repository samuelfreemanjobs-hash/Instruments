# Supabase — Black Friday Madness

## Local / hosted

1. Create a project at [supabase.com](https://supabase.com).
2. Install CLI: `npm i -g supabase` (optional).
3. Link: `supabase link --project-ref <ref>`.
4. Apply migrations in order:

```bash
cd tools/deal-hunter
supabase db push
# or run SQL manually in Dashboard → SQL Editor:
#   migrations/001_black_friday_madness.sql
#   migrations/002_auth_rls_user_data.sql
#   migrations/003_sync_constraints.sql
#   migrations/005_shopping_budget.sql
#   migrations/004_notification_preferences.sql
#   migrations/006_catalog_public_read.sql
#   migrations/007_user_push_subscriptions.sql
```

5. Auth → URL configuration: add redirect `http://localhost:3000/auth/callback` and production URL.

## Env (Next.js `web/.env.local`)

```bash
NEXT_PUBLIC_SUPABASE_URL=https://xxxx.supabase.co
NEXT_PUBLIC_SUPABASE_ANON_KEY=eyJ...
DEAL_API_ORIGIN=http://127.0.0.1:3847
```

Service role key stays on the **API worker only** — never `NEXT_PUBLIC_*`.

## Catalog sync

```bash
cd tools/deal-hunter
npm run sync:supabase
```

Populates `retailers`, `products`, `product_offers`, `deal_records` from the live demo catalog.

## RLS

- User tables: own-row only (`auth.uid()`).
- `deal_records`: public read for `status = active` catalog sync (future).
