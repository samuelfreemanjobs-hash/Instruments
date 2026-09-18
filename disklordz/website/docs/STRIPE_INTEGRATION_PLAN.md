# Stripe integration plan — DISKLORDZ.COM

Fallback when **`stripe_implementation_planner`** (Stripe MCP) is not available. Aligns with current code in `disklordz/website`.

## Business context

| | |
|--|--|
| **Brand** | DISKLORDZ.COM |
| **Offer** | Drum kit SaaS (generations + ZIP); plugins lane separate |
| **Payments (now)** | Monthly **Pro** subscription — unlimited generations |
| **Payments (later)** | One-time **product pack** SKUs, plugin licenses |

## Phase 1 — Live today (WO-SAAS-010)

| Step | Stripe | App |
|------|--------|-----|
| Product | Product “Disklordz Pro” + recurring Price | `STRIPE_PRO_PRICE_ID` |
| Subscribe | Checkout Session `mode: subscription` | `POST /api/stripe/checkout` |
| Entitlement | Webhook → set `plan = pro` | `POST /api/stripe/webhook` |
| Usage | Credits for free; Pro bypasses spend RPC | `credits.ts` + `user_billing` |

**Webhook events (configure in Dashboard):**

- `checkout.session.completed`
- `customer.subscription.updated`
- `customer.subscription.deleted`

**Metadata:** `user_id` (Supabase auth uuid) on Checkout Session and `subscription_data.metadata`.

## Phase 2 — Hardening (recommended)

| Item | Why | Status in repo |
|------|-----|----------------|
| Reuse `stripe_customer_id` on repeat Checkout | Avoid duplicate customers | Implemented in checkout route |
| **Customer Portal** | Cancel/update card without custom UI | `POST /api/stripe/portal` |
| Handle `past_due` | Downgrade or grace period | Extend `subscription.updated` handler |
| Webhook idempotency | Safe retries | Use `event.id` in ledger/idempotency table (future) |
| `customer.subscription.created` | Redundant with checkout.completed | Optional |
| Test vs live | Separate Vercel Preview env keys | Manual in Vercel |

## Phase 3 — Future SKUs

| SKU | Stripe model | App touchpoints |
|-----|--------------|-----------------|
| Product factory pack | One-time Payment or Checkout `mode: payment` | New price id; optional webhook to grant credits |
| Plugin license | Payment Link or Checkout + license key | New product; not in website v0 |

## Security checklist

- Webhook: always `constructEvent` with raw body + `STRIPE_WEBHOOK_SECRET`.
- Never expose `STRIPE_SECRET_KEY` or service role to the client.
- Restricted keys (`rk_...`) only for MCP/agents, not Vercel production unless scoped minimally.
- Enable Stripe MCP in Dashboard for the account used by OAuth.

## Verification

1. Test Checkout → success URL `/account?checkout=success`.
2. Stripe CLI or Dashboard → replay webhook → `user_billing.plan = pro`.
3. Generate kit → credits not decremented for Pro.
4. Portal → cancel → webhook → `plan = free`.

Related: [STRIPE.md](STRIPE.md) · [STRIPE_MCP.md](STRIPE_MCP.md)
