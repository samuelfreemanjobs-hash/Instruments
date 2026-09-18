# Go-live secrets (one-time setup)

Add these under **GitHub → Settings → Secrets and variables → Actions → Repository secrets** to run the automated go-live workflow.

## Required for full automation

| Secret | Used for |
|--------|----------|
| `SUPABASE_ACCESS_TOKEN` | `supabase db push` |
| `SUPABASE_PROJECT_REF` | Supabase project id (subdomain) |
| `DISKLORDZ_URL` | Production site URL, no trailing slash (e.g. `https://disklordz.vercel.app`) |
| `VERCEL_DEPLOY_HOOK_URL` | Vercel → Project → Settings → Git → Deploy Hooks → Production hook URL |

## Required to sync env vars to Vercel (workflow step)

| Secret | Maps to Vercel env |
|--------|---------------------|
| `VERCEL_TOKEN` | [Vercel account token](https://vercel.com/account/tokens) |
| `VERCEL_PROJECT_ID` | Project Settings → General → Project ID |
| `NEXT_PUBLIC_SUPABASE_URL` | Same name on Vercel |
| `NEXT_PUBLIC_SUPABASE_ANON_KEY` | Same name |
| `SUPABASE_SERVICE_ROLE_KEY` | Same name |
| `STRIPE_SECRET_KEY` | Same name |
| `STRIPE_WEBHOOK_SECRET` | Same name (create webhook first — see below) |
| `STRIPE_PRO_PRICE_ID` | Same name |

## Optional

| Secret | Default |
|--------|---------|
| `SAAS_DAILY_GEN_LIMIT` | `20` |
| `VERCEL_TEAM_ID` | Only if project is under a team |

## Stripe webhook secret (first time)

Either create manually in Stripe Dashboard, or run locally:

```bash
cd disklordz/website
export STRIPE_SECRET_KEY=sk_test_...
export DISKLORDZ_URL=https://your-production-url
bash scripts/setup-stripe-webhook.sh
```

Copy the printed `whsec_...` into GitHub secret `STRIPE_WEBHOOK_SECRET` and Vercel env, then re-run go-live with **Sync Vercel env** enabled.

## Run automation

**Actions → Disklordz go-live → Run workflow**

Local equivalent: `cd disklordz/website && bash scripts/go-live.sh`
