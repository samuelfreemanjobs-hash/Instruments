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

## Stripe MCP (Cursor agents)

[STRIPE_MCP.md](STRIPE_MCP.md) — plugin + `https://mcp.stripe.com` + `stripe_implementation_planner`.  
Integration plan: [STRIPE_INTEGRATION_PLAN.md](STRIPE_INTEGRATION_PLAN.md).

## Stripe Dashboard

1. Enable **Customer portal** (Settings → Billing → Customer portal) for cancel/update payment method.
2. Create a **Product** → recurring **Price** (monthly Pro).
3. Copy Price ID → `STRIPE_PRO_PRICE_ID`.
4. Developers → Webhooks → endpoint  
   `https://YOUR_DOMAIN/api/stripe/webhook`  
   Events: `checkout.session.completed`, `customer.subscription.updated`, `customer.subscription.deleted`.
5. Copy signing secret → `STRIPE_WEBHOOK_SECRET`.

## SQL

Run `supabase/migrations/20260917220000_credits_billing.sql` after saved_kits migration.

## Local webhook test

```bash
stripe listen --forward-to localhost:3000/api/stripe/webhook
```

Use test keys and a test Price ID in `.env.local`.
