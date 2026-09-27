# Finish Disklordz Drum SaaS (ship checklist)

**Code status:** Feature-complete on `main` (WO-SAAS-001–016). **Remaining work is operational** — Vercel, Supabase prod, Stripe live/test, secrets, smoke.

Market positioning: [`DISKLORDZ_MARKET_INTELLIGENCE.md`](DISKLORDZ_MARKET_INTELLIGENCE.md) (if merged) · Product definition: [`DISKLORDZ_SAAS_V0.md`](DISKLORDZ_SAAS_V0.md).

---

## What “finished” means

| Layer | Done when |
|-------|-----------|
| **Product** | Guest can generate → preview → ZIP; signed-in saves kits; product pack export; Pro checkout works |
| **Ops** | HTTPS production URL, migrations applied, auth redirect URLs, webhook delivering |
| **Business** | Planner approved price tier; Marketing has hero pack + tutorial thread |

---

## Step 1 — Verify code locally (5 min)

```bash
cd disklordz/website
npm ci && npm run build
npm run start &
sleep 3
DISKLORDZ_URL=http://127.0.0.1:3000 npm run verify:go-live
curl -s http://127.0.0.1:3000/api/health | python3 -m json.tool
```

Expect `verify:go-live` **All automated checks passed** and `/api/health` `status: ok` (billing flags `false` until Stripe env is set locally).

---

## Step 2 — One-time GitHub secrets

Follow [`DISKLORDZ_GO_LIVE_SECRETS.md`](DISKLORDZ_GO_LIVE_SECRETS.md):

- `SUPABASE_ACCESS_TOKEN`, `SUPABASE_PROJECT_REF`
- `DISKLORDZ_URL` (production URL, no trailing slash)
- `VERCEL_DEPLOY_HOOK_URL`
- Optional sync: `VERCEL_TOKEN`, `VERCEL_PROJECT_ID`, all env vars listed there

---

## Step 3 — Vercel project

1. [vercel.com](https://vercel.com) → Import **Instruments** → **Root Directory** = `disklordz/website`
2. Set Production env vars ([`DEPLOY.md`](../disklordz/website/DEPLOY.md))
3. Create **Deploy Hook** → save URL as `VERCEL_DEPLOY_HOOK_URL`

---

## Step 4 — Supabase

1. **Authentication → URL configuration:** Site URL + `https://YOUR_DOMAIN/auth/callback`
2. Run migrations (order in [`DISKLORDZ_GO_LIVE.md`](DISKLORDZ_GO_LIVE.md) §4)  
   - **GitHub:** Actions → **Disklordz Supabase migrations** → Run workflow  
   - **Local:** `bash disklordz/website/scripts/apply-supabase-migrations.sh`

---

## Step 5 — Stripe Pro

1. Create recurring **Pro** price → `STRIPE_PRO_PRICE_ID`
2. Webhook → `https://YOUR_DOMAIN/api/stripe/webhook`  
   Events: `checkout.session.completed`, `customer.subscription.updated`, `customer.subscription.deleted`  
   Details: [`disklordz/website/docs/STRIPE.md`](../disklordz/website/docs/STRIPE.md)
3. `bash disklordz/website/scripts/setup-stripe-webhook.sh` (optional) → copy `whsec_` to Vercel + GitHub

---

## Step 6 — Automated go-live (recommended)

**GitHub → Actions → Disklordz go-live → Run workflow**

- Enable **Sync Vercel env** if secrets are in GitHub
- Enable **Register Stripe webhook** on first run if needed
- Workflow runs migrations → deploy hook → `verify:go-live` against `DISKLORDZ_URL`

Local mirror:

```bash
cd disklordz/website
export DISKLORDZ_URL=https://your-app.vercel.app
export VERCEL_DEPLOY_HOOK_URL=...
bash scripts/go-live.sh --sync-vercel --stripe-webhook
```

---

## Step 7 — Manual smoke (you, 10 min)

On production URL ([`DISKLORDZ_GO_LIVE.md`](DISKLORDZ_GO_LIVE.md) §6):

1. Guest one-shot + ZIP  
2. Loop mode in spec  
3. Magic-link sign-in → `/account` credits  
4. Product factory pack ZIP (`01_KICKS` …)  
5. Stripe test checkout → Pro → unlimited gens  

---

## Step 8 — Post-ship

- Airtable WOs **007–016** → Done  
- Marketing: launch thread + MPC handoff ([`DISKLORDZ_SAAS_AGENT_LANES.md`](DISKLORDZ_SAAS_AGENT_LANES.md))  
- Monitor `/api/health` (`billingReady: true` when fully wired)

---

## What agents should / should not do

| Agent | Role |
|-------|------|
| **Cursor Cloud** | Code fixes only if smoke fails — not production deploy unless you ask |
| **You / THOR** | Secrets, Vercel, Stripe live toggle |
| **Business Planner** | Approve go-live + Pro price ($12.99–19.99/mo band per market brief) |

**Do not** force-push production or enable live Stripe without explicit approval ([`AGENTS.md`](../AGENTS.md)).
