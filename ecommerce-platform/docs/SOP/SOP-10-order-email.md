# SOP-10 — Order confirmation email

## Steps

1. Copy `templates/email/order-confirmation.template.tsx` (React Email or HTML string variant).
2. Trigger from webhook handler or order status transition to `PAID`.
3. Include: order id, line items, total, shipping address.
4. Use SMTP or provider API from env (`SMTP_*` or `RESEND_API_KEY`).

## Definition of Done

- [ ] Customer receives email on successful payment in dev (Mailtrap or provider logs)

## Verification

Place test order → check inbox or provider dashboard.
