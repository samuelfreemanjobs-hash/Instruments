# SOP-00 — Prerequisites & tech stack

## Tech stack

| Layer | Choice |
|-------|--------|
| Framework | Next.js 15 App Router, TypeScript |
| Styling | Tailwind CSS |
| ORM | Prisma |
| Database | MongoDB |
| Forms | React Hook Form + Zod |
| Payments | Stripe + RazorPay |
| Media | Cloudinary |
| AI IDE | Cursor (or Trae) + ecommerce-builder skill |

## Accounts to create

1. MongoDB Atlas — free cluster, connection string for `DATABASE_URL`
2. Stripe — test keys + webhook secret
3. RazorPay — test keys (optional if Stripe-only)
4. Cloudinary — cloud name, API key/secret
5. SMTP or Resend/SendGrid for order emails

## Local setup

```bash
node -v   # v20+
cd ecommerce-platform/web
cp .env.example .env.local
npm install
```

## Definition of Done

- [ ] `.env.local` exists with placeholder keys documented
- [ ] `npm run dev` starts without missing-module errors

## Verification

```bash
cd ecommerce-platform/web && npm run build
```
