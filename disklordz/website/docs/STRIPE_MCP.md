# Stripe MCP (Cursor) — DISKLORDZ.COM

**Business:** Audio plugins and drum kits · **Payments:** subscriptions (Pro) + future one-time product packs.

## 1. Stripe plugin (preferred)

In Cursor:

1. **Customize → Plugins** (or command palette: add Stripe plugin).
2. Install from [Cursor Marketplace — Stripe](https://cursor.com/marketplace/stripe).
3. Reload tools or start a **new chat** if tools do not appear.

The plugin registers the hosted MCP server and OAuth.

## 2. Repo MCP config (already in git)

[`.cursor/mcp.json`](../../../.cursor/mcp.json) includes:

```json
"stripe": {
  "type": "http",
  "url": "https://mcp.stripe.com"
}
```

Optional header (headless / no OAuth): `Authorization: Bearer ${env:STRIPE_RESTRICTED_KEY}` using a **restricted** key from Stripe Dashboard → Developers → API keys → Create restricted key (enable only what agents need).

**Do not** commit `sk_live_` or unrestricted keys.

### Connect

1. **Cursor Settings → MCP** → **stripe** → **Connect** (OAuth) — pick the Stripe account for DISKLORDZ.
2. Confirm tools such as **`stripe_implementation_planner`** appear after reload.

Official docs: [Stripe MCP](https://docs.stripe.com/mcp) · [Cursor MCP](https://cursor.com/docs/mcp).

## 3. Generate an integration plan (in Cursor)

In a **new** agent chat (with Stripe MCP connected), ask:

> Use `stripe_implementation_planner` for: Business DISKLORDZ.COM — audio plugins and drum kits SaaS; need subscription Pro plan, Checkout, webhooks syncing to Supabase `user_billing`, optional Customer Portal; stack Next.js on Vercel.

Repo fallback (if the tool is unavailable): [STRIPE_INTEGRATION_PLAN.md](STRIPE_INTEGRATION_PLAN.md).

## 4. API keys for the live app (Vercel)

MCP OAuth is for **agents in the IDE**. Production still uses env vars:

| Vercel env | Purpose |
|------------|---------|
| `STRIPE_SECRET_KEY` | Server API (`sk_test_` / `sk_live_`) |
| `STRIPE_WEBHOOK_SECRET` | `whsec_...` from webhook endpoint |
| `STRIPE_PRO_PRICE_ID` | Recurring `price_...` for Pro |

See [STRIPE.md](STRIPE.md) and [VERCEL_ENV_AND_MCP.md](VERCEL_ENV_AND_MCP.md).

## 5. Cloud Agents

Add secret `STRIPE_RESTRICTED_KEY` (optional) for MCP; allow egress to **`mcp.stripe.com`**. Production deploy keys stay in **Vercel**, not the agent VM.

## 6. Local webhook testing

```bash
stripe listen --forward-to localhost:3000/api/stripe/webhook
```

Use test mode keys in `.env.local`.
