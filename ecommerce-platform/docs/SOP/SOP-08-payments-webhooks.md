# SOP-08 — Stripe & RazorPay webhooks

## Stripe

1. Copy `templates/api/stripe-checkout.route.ts` → `app/api/payments/stripe/checkout/route.ts`.
2. Webhook: `templates/api/stripe-webhook.route.ts` — verify signature with `STRIPE_WEBHOOK_SECRET`.
3. On `checkout.session.completed`, set order `PAID`.

## RazorPay

1. Create order server-side with `templates/lib/razorpay.template.ts`.
2. Webhook template verifies signature; updates same `Order` model.

## Env vars

See `web/.env.example` — `STRIPE_*`, `RAZORPAY_*`.

## Definition of Done

- [ ] Test payment succeeds in Stripe test mode
- [ ] Webhook idempotent (duplicate events do not double-ship)

## Verification

Stripe CLI: `stripe listen --forward-to localhost:3000/api/webhooks/stripe`
