# Stripe Pro billing (WO-SAAS-010)

## Products

| Plan | Generations |
|------|-------------|
| **Guest** | IP daily cap (`SAAS_DAILY_GEN_LIMIT`, default 20) |
| **Free (signed in)** | 50 credits on first billing row; 1 credit = 1 variation batch |
| **Pro** | Unlimited (`plan = pro` in `user_billing`) |

## Vercel env

```bash
STRIPE_SECRET_KEY=sk_live_...
STRIPE_WEBHOOK_SECRET=whsec_...
STRIPE_PRO_PRICE_ID=price_...
```

Requires `SUPABASE_SERVICE_ROLE_KEY` for credit spend + webhook updates.

## Stripe Dashboard

1. Create a **Product** → recurring **Price** (monthly Pro).
2. Copy Price ID → `STRIPE_PRO_PRICE_ID`.
3. Developers → Webhooks → endpoint  
   `https://YOUR_DOMAIN/api/stripe/webhook`  
   Events: `checkout.session.completed`, `customer.subscription.updated`, `customer.subscription.deleted`.
4. Copy signing secret → `STRIPE_WEBHOOK_SECRET`.

## SQL

Run `supabase/migrations/20260917220000_credits_billing.sql` after saved_kits migration.

## Local webhook test

```bash
stripe listen --forward-to localhost:3000/api/stripe/webhook
```

Use test keys and a test Price ID in `.env.local`.
