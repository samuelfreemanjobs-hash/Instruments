# Payments — Stripe & RazorPay (SOP-08)

## Flow

1. Customer completes **checkout** → `Order` status `PENDING`.
2. On `/account/orders/[id]`, customer pays via **Stripe Checkout** or **RazorPay**.
3. Webhook (or RazorPay client verify) calls `fulfillOrderPayment()` → status `PAID`, stock decremented **once** (idempotent).

## Stripe

`.env.local`:

```env
STRIPE_SECRET_KEY=sk_test_...
STRIPE_WEBHOOK_SECRET=whsec_...
NEXT_PUBLIC_STRIPE_PUBLISHABLE_KEY=pk_test_...
NEXT_PUBLIC_APP_URL=http://localhost:3000
```

Local webhooks:

```bash
stripe listen --forward-to localhost:3000/api/webhooks/stripe
```

Use the printed `whsec_...` as `STRIPE_WEBHOOK_SECRET`.

Events handled: `checkout.session.completed` (metadata `orderId`).

## RazorPay

```env
RAZORPAY_KEY_ID=rzp_test_...
RAZORPAY_KEY_SECRET=...
RAZORPAY_WEBHOOK_SECRET=...   # optional; falls back to KEY_SECRET for webhook HMAC
```

- `POST /api/payments/razorpay/order` — creates RazorPay order, stores id on `Order.paymentRef`
- Client checkout → `POST /api/payments/razorpay/verify`
- `POST /api/webhooks/razorpay` — optional `payment.captured` handler

## Development without keys

On order detail, **Simulate payment (dev)** appears when `NODE_ENV !== production` and Stripe is not configured. It runs the same fulfillment path as a successful pay (without external API).

## Security

- Never expose `STRIPE_SECRET_KEY` or `RAZORPAY_KEY_SECRET` to the client.
- Webhook routes verify signatures before updating orders.
