# Disklordz Drum SaaS — go-live checklist

Use this after **WO-SAAS-007–015** land on `main`. The Cloud Agent cannot access your Vercel/Supabase dashboards; these steps are **owner actions** with repo-backed verification at the end.

## 1. Merge and deploy

| Step | Action |
|------|--------|
| 1.1 | Review and **merge** [PR #28](https://github.com/samuelfreemanjobs-hash/Instruments/pull/28) (or current SaaS PR) into `main`. |
| 1.2 | **Vercel** — import or link repo; **Root Directory** = `disklordz/website`. |
| 1.3 | Confirm production deploy from `main` (GitHub Action `Disklordz website` must pass on merge). |

## 2. Vercel environment variables

Set for **Production** (and Preview if you test billing there):

| Variable | Required | Notes |
|----------|----------|--------|
| `NEXT_PUBLIC_SUPABASE_URL` | Yes | Supabase project URL |
| `NEXT_PUBLIC_SUPABASE_ANON_KEY` | Yes | Client auth |
| `SUPABASE_SERVICE_ROLE_KEY` | **Yes in prod** | Kits storage + credits; never expose to client |
| `STRIPE_SECRET_KEY` | For Pro | Test vs live keys per environment |
| `STRIPE_WEBHOOK_SECRET` | For Pro | Endpoint: `https://YOUR_DOMAIN/api/stripe/webhook` |
| `STRIPE_PRO_PRICE_ID` | For Pro | Recurring price id |
| `SAAS_DAILY_GEN_LIMIT` | Optional | Guest IP batches/day (default `20`) |

Details: [disklordz/website/DEPLOY.md](../disklordz/website/DEPLOY.md), [disklordz/website/docs/STRIPE.md](../disklordz/website/docs/STRIPE.md).

## 3. Supabase Auth URLs

In **Authentication → URL configuration**:

- **Site URL:** `https://YOUR_DOMAIN`
- **Redirect URLs:** `https://YOUR_DOMAIN/auth/callback`

## 4. SQL migrations (order matters)

Run in the Supabase SQL editor (or CLI) **in this order**:

1. `disklordz/website/supabase/migrations/20260917190000_saved_kits.sql`
2. `disklordz/website/supabase/migrations/20260917210000_kits_storage_bucket.sql`
3. `disklordz/website/supabase/migrations/20260917220000_credits_billing.sql`
4. `disklordz/website/supabase/migrations/20260917230000_saved_kits_history.sql`
5. `disklordz/website/supabase/migrations/20260917240000_credits_variable_amount.sql` — **loop/SFX/pack credit amounts**

## 5. Stripe webhook

1. Create webhook pointing to `https://YOUR_DOMAIN/api/stripe/webhook`.
2. Events: `checkout.session.completed`, `customer.subscription.updated`, `customer.subscription.deleted` (see STRIPE.md).
3. Paste signing secret into Vercel as `STRIPE_WEBHOOK_SECRET`.

## 6. Production smoke test (manual)

On **https://YOUR_DOMAIN**:

1. **Guest** — generate a one-shot kit; preview kick; download ZIP.
2. **Guest** — loop mode in spec; confirm `loop_main.wav` in ZIP.
3. **Sign in** — magic link; confirm credits balance on home.
4. **Signed in** — generate; confirm kit appears on `/account`.
5. **Product factory** — build pack; download ZIP; folders `01_KICKS` … `04_PERC`.
6. **Pro (test mode)** — checkout; webhook fires; unlimited generations.

## 7. Automated smoke (optional)

From your machine (or CI secret `DISKLORDZ_URL`):

```bash
cd disklordz/website
DISKLORDZ_URL=https://YOUR_DOMAIN npm run verify:go-live
```

Exits non-zero if health checks fail. Does **not** replace signed-in or Stripe tests.

## 8. Post-go-live

- Mark Airtable WOs **007–015** Done.
- Optional next build: **WO-SAAS-016** DAW inbox — [disklordz/daw-inbox/README.md](../disklordz/daw-inbox/README.md).
