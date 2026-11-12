# Black Friday Madness — go-live secrets (GitHub Actions)

One-time setup in **GitHub → Settings → Secrets and variables → Actions → Repository secrets**.

Then run **Actions → “Black Friday Madness go-live” → Run workflow** from a phone or browser (no laptop required).

## Required

| Secret | Purpose |
|--------|---------|
| `SUPABASE_ACCESS_TOKEN` | [Account access token](https://supabase.com/dashboard/account/tokens) for `supabase db push` |
| `SUPABASE_PROJECT_REF` | Supabase project id (subdomain in `https://<ref>.supabase.co`) |
| `SUPABASE_SERVICE_ROLE_KEY` | **Settings → API → service_role** (server-only; used for catalog sync in CI) |

## Recommended

| Secret | Purpose |
|--------|---------|
| `BFM_LIVE_URL` | Published app origin, no trailing slash. Default: `https://black-friday-madness.apps.grok.bot` |
| `BFM_SUPABASE_URL` | Full API URL if not `https://${SUPABASE_PROJECT_REF}.supabase.co` |

## Already in Cursor (not GitHub)

Publish **Secrets** (not chat): `SUPABASE_URL`, `SUPABASE_ANON_KEY`, **`USE_SUPABASE_CATALOG=1`**.

After the workflow finishes, **republish** black-friday-madness only if you changed Cursor secrets. The workflow does not replace republish.

## What the workflow does

1. `supabase db push` (deal-hunter migrations)
2. Sets Supabase Auth **site URL** + redirect for `BFM_LIVE_URL` (Management API)
3. `npm run sync:supabase` (fills `product_offers` / `deal_records`)
4. Smoke test: live `/api/catalog/source` reports `useSupabase: true`

## Local equivalent

```bash
cd tools/deal-hunter
export SUPABASE_ACCESS_TOKEN=...
export SUPABASE_PROJECT_REF=...
export SUPABASE_SERVICE_ROLE_KEY=...
npm run go-live
```
