# SOP-03 — Prisma ORM & models

## Steps

1. Use `web/prisma/schema.prisma` (or `templates/prisma/schema.prisma.template`).
2. Set `provider = "mongodb"` and `url = env("DATABASE_URL")`.
3. Run:

```bash
cd web
npx prisma generate
npx prisma db push
```

4. Optional seed: `templates/scripts/seed-admin.template.ts` → `prisma/seed.ts`, register in `package.json`.

## Model checklist

- User (email, passwordHash, role)
- Category, SubCategory
- Product (slug, price, images[], stock, relations)
- Coupon
- Order, OrderItem
- Review
- Banner (type: WEBSITE | APP)
- TopBar, Offer

## Definition of Done

- [ ] `prisma generate` succeeds
- [ ] `db push` succeeds against Atlas
- [ ] At least one document creatable via Prisma Studio or seed

## Verification

```bash
npx prisma studio
```
