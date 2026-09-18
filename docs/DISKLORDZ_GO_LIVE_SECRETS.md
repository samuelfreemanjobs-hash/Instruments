# Go-live secrets — one-time setup (~10 min)

**Where:** GitHub repo **Instruments** → **Settings** → **Secrets and variables** → **Actions** → **New repository secret**

Use the **exact** secret names below (case-sensitive).

---

## Checklist A — minimum (migrations + deploy + smoke)

Add these **four** secrets first. You can run the workflow without Vercel env sync.

| # | Secret name | Where to copy the value |
|---|-------------|-------------------------|
| 1 | `SUPABASE_ACCESS_TOKEN` | [Supabase → Account → Access tokens](https://supabase.com/dashboard/account/tokens) → Generate |
| 2 | `SUPABASE_PROJECT_REF` | Supabase project URL: `https://**abcdefghijklmnop**.supabase.co` → use `abcdefghijklmnop` |
| 3 | `DISKLORDZ_URL` | Vercel → your project → **Domains** or deployment URL, e.g. `https://instruments-xxx.vercel.app` — **no** trailing `/` |
| 4 | `VERCEL_DEPLOY_HOOK_URL` | Vercel → Project → **Settings → Git → Deploy Hooks** → Create → **Production** → copy hook URL |

**Run:** Actions → **Disklordz go-live** → Run workflow  
- Sync Vercel env: **off** (until Checklist B is done)  
- Register Stripe webhook: **off** (until Checklist C)  
- Leave other skips **off**

---

## Checklist B — sync env to Vercel (recommended)

Add these **eight** secrets so the workflow can push env vars to Vercel (no manual paste in Vercel UI).

| # | Secret name | Where to copy the value |
|---|-------------|-------------------------|
| 5 | `VERCEL_TOKEN` | [vercel.com/account/tokens](https://vercel.com/account/tokens) → Create |
| 6 | `VERCEL_PROJECT_ID` | Vercel → Project → **Settings → General** → **Project ID** |
| 7 | `NEXT_PUBLIC_SUPABASE_URL` | Supabase → **Project Settings → API** → Project URL |
| 8 | `NEXT_PUBLIC_SUPABASE_ANON_KEY` | Same page → `anon` `public` key |
| 9 | `SUPABASE_SERVICE_ROLE_KEY` | Same page → `service_role` **secret** (never use in client code) |
| 10 | `STRIPE_SECRET_KEY` | Stripe → **Developers → API keys** → Secret key (`sk_test_` or `sk_live_`) |
| 11 | `STRIPE_PRO_PRICE_ID` | Stripe → **Product** → Pro → **Price** id `price_...` |
| 12 | `STRIPE_WEBHOOK_SECRET` | See **Checklist C** first (`whsec_...`) |

Optional:

| Secret | Default |
|--------|---------|
| `SAAS_DAILY_GEN_LIMIT` | `20` |
| `VERCEL_TEAM_ID` | Vercel team id — only if the project is under a team |

**Run:** Actions → **Disklordz go-live** → **Sync Vercel env: on**

---

## Checklist C — Stripe webhook secret (first time only)

You need `STRIPE_WEBHOOK_SECRET` before sync can configure Stripe on Vercel.

**Option 1 — GitHub workflow**

1. Ensure secrets `STRIPE_SECRET_KEY` and `DISKLORDZ_URL` exist.
2. Actions → **Disklordz go-live** → **Register Stripe webhook: on** (sync can stay off).
3. Open the job log → step **Register Stripe webhook** → copy `STRIPE_WEBHOOK_SECRET=whsec_...`
4. Add GitHub secret `STRIPE_WEBHOOK_SECRET` with that value (optional: same value in Vercel → **Settings → Environment Variables** if you deploy before sync).
5. Run again with **Sync Vercel env: on**, **Register Stripe webhook: off** (sync copies `whsec` to Vercel).

**Option 2 — local script**

```bash
cd disklordz/website
export STRIPE_SECRET_KEY=sk_test_...
export DISKLORDZ_URL=https://your-vercel-url
bash scripts/setup-stripe-webhook.sh
```

Then add `STRIPE_WEBHOOK_SECRET` to GitHub (and re-run go-live with sync on).

**Manual:** Stripe → **Developers → Webhooks** → endpoint `https://YOUR_DOMAIN/api/stripe/webhook` → events: `checkout.session.completed`, `customer.subscription.updated`, `customer.subscription.deleted` → signing secret → `STRIPE_WEBHOOK_SECRET`.

---

## After secrets: two-run summary

| Run | Sync Vercel env | Register Stripe webhook |
|-----|-----------------|-------------------------|
| **1** (if no `whsec` yet) | off | **on** → copy `whsec` → add secret |
| **2** (full go-live) | **on** | off |

Each run should: migrate DB → (optional sync) → deploy hook → **green smoke test**.

---

## Still manual (not GitHub secrets)

| Task | Where |
|------|--------|
| Auth redirect | Supabase → **Authentication → URL configuration** → `https://YOUR_DOMAIN/auth/callback` |
| Customer portal | Stripe → **Settings → Billing → Customer portal** → Enable |
| Cursor MCP | Your Mac — [SUPABASE_MCP.md](../disklordz/website/docs/SUPABASE_MCP.md), [STRIPE_MCP.md](../disklordz/website/docs/STRIPE_MCP.md) |

---

## Local (same as Actions)

```bash
cd disklordz/website
export SUPABASE_ACCESS_TOKEN=... SUPABASE_PROJECT_REF=...
export DISKLORDZ_URL=... VERCEL_DEPLOY_HOOK_URL=...
# optional: all Vercel/Stripe vars for --sync-vercel
bash scripts/go-live.sh --sync-vercel --stripe-webhook   # first time
bash scripts/go-live.sh --sync-vercel                    # after whsec is set
```

See also [DISKLORDZ_GO_LIVE.md](DISKLORDZ_GO_LIVE.md).
