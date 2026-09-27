# Finish Disklordz Drum SaaS (ship checklist)

**Code:** complete on `main` (WO-SAAS-001–016). **Finish** = run the automation below with your secrets.

**CLI (all commands):**

```bash
# From repo root
./scripts/disklordz-saas.sh help

# Or from website
cd disklordz/website && npm run saas -- help
```

---

## Quick paths

| Goal | Command |
|------|---------|
| First local dev | `npm run saas -- dev-setup` then `npm run dev` |
| Prove app works (no cloud) | `npm run smoke:local` |
| Check you have secrets | `npm run saas -- preflight` |
| List GitHub secret names | `npm run saas -- gh-secrets` |
| Print `gh secret set` commands | `npm run saas -- gh-secret-cmds` |
| **Full production go-live** | See §Full go-live |

---

## One-time: env file (local automation)

```bash
cd disklordz/website
cp .env.go-live.example .env.go-live
# Edit .env.go-live — never commit
```

Scripts auto-load `.env.go-live` via `load-go-live-env.sh`.

---

## One-time: GitHub Actions secrets

```bash
cd disklordz/website
npm run saas -- gh-secret-cmds   # paste values when prompted
npm run saas -- gh-secrets       # verify names exist
```

Table: [`DISKLORDZ_GO_LIVE_SECRETS.md`](DISKLORDZ_GO_LIVE_SECRETS.md).

---

## One-time: Vercel

1. Import **Instruments** → **Root Directory** `disklordz/website`
2. Create **Production Deploy Hook** → `VERCEL_DEPLOY_HOOK_URL`
3. Either:
   - **GitHub Actions** sync: `VERCEL_TOKEN` + `VERCEL_PROJECT_ID` + env secrets, or
   - **Interactive:** `npm run saas -- vercel-env-interactive` (requires `vercel link`)

---

## One-time: Stripe Pro (scripted)

With `STRIPE_SECRET_KEY` in `.env.go-live`:

```bash
npm run saas -- stripe-price    # prints STRIPE_PRO_PRICE_ID
# Add price id to .env.go-live + GitHub + Vercel
npm run saas -- stripe-webhook  # prints STRIPE_WEBHOOK_SECRET once
```

Details: [`disklordz/website/docs/STRIPE.md`](../disklordz/website/docs/STRIPE.md).

---

## Full go-live (local)

```bash
cd disklordz/website
npm run saas -- go-live --sync-vercel --configure-auth --stripe-webhook
```

Flags: `npm run saas -- go-live --help`

Steps executed:

1. `preflight-go-live.sh` (production)
2. Optional `--stripe-price`
3. `apply-supabase-migrations.sh`
4. Optional `--configure-auth` (Management API)
5. Optional `--stripe-webhook`
6. Optional `--sync-vercel`
7. Deploy hook POST
8. `verify-go-live.sh` (+ `--require-accounts` / billing when Stripe set)

---

## Full go-live (GitHub Actions)

**Actions → Disklordz go-live → Run workflow**

- Enable **Configure Supabase auth URLs**
- Enable **Sync Vercel env**
- Enable **Register Stripe webhook** on first run

Same as local; secrets live in GitHub.

---

## Verify production

```bash
export DISKLORDZ_URL=https://your-app.vercel.app
npm run verify:go-live -- --require-accounts --require-billing
curl -s "$DISKLORDZ_URL/api/health" | python3 -m json.tool
```

Manual: [`DISKLORDZ_GO_LIVE.md`](DISKLORDZ_GO_LIVE.md) §6 (sign-in, Stripe test checkout).

---

## Script reference

| Script | Purpose |
|--------|---------|
| `scripts/saas.sh` | Master CLI |
| `scripts/dev-setup.sh` | Local deps + `.env.local` |
| `scripts/smoke-local.sh` | Build + smoke (CI uses this) |
| `scripts/preflight-go-live.sh` | Tooling + env validation |
| `scripts/apply-supabase-migrations.sh` | `supabase db push` |
| `scripts/configure-supabase-auth.sh` | Auth site URL + callback |
| `scripts/setup-stripe-pro-price.sh` | Create Pro price |
| `scripts/setup-stripe-webhook.sh` | Register webhook |
| `scripts/sync-vercel-env.sh` | Vercel REST env upsert |
| `scripts/push-vercel-env.sh` | Interactive `vercel env add` |
| `scripts/verify-go-live.sh` | HTTP smoke + ZIP download |
| `scripts/go-live.sh` | Orchestrator |
| `scripts/check-github-secrets.sh` | `gh secret list` diff |
| `scripts/print-secret-set-commands.sh` | `gh secret set` template |

Repo root: [`scripts/disklordz-saas.sh`](../scripts/disklordz-saas.sh).

---

## Post-ship

- Airtable WOs 007–016 → Done  
- Marketing launch ([`DISKLORDZ_SAAS_AGENT_LANES.md`](DISKLORDZ_SAAS_AGENT_LANES.md))  
- Market intel refresh: [`DISKLORDZ_MARKET_INTELLIGENCE.md`](DISKLORDZ_MARKET_INTELLIGENCE.md) (if merged)

Agents: **do not** deploy production or enable live Stripe without explicit user request.
