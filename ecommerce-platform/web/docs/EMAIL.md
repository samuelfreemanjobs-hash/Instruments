# Order confirmation email (SOP-10)

## When it sends

After `fulfillOrderPayment()` successfully marks an order **PAID** (Stripe webhook, RazorPay verify, or dev simulate), the app calls `sendOrderConfirmationEmail(orderId)`.

## Environment (SMTP)

```env
SMTP_HOST=smtp.example.com
SMTP_PORT=587
SMTP_USER=...
SMTP_PASS=...
EMAIL_FROM="Cursor Shop <orders@yourshop.com>"
```

Use Mailtrap, SendGrid SMTP, Gmail app password, etc.

## Development without SMTP

If SMTP is not configured and `NODE_ENV !== production`, the email **body is logged to the server console** (`[email:dev]`) so you can verify content without sending.

## Content

Includes line items, totals, shipping address, and link to `/account/orders/[id]`.
