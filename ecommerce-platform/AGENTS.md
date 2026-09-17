# AGENTS.md — eCommerce Platform

## Before you edit

1. Read [/ARCHITECTURE.md](../ARCHITECTURE.md) (repo index), then [ARCHITECTURE.md](ARCHITECTURE.md).
2. Identify the **blueprint phase** in [docs/BLUEPRINT.md](docs/BLUEPRINT.md) and open the matching **SOP** in [docs/SOP/](docs/SOP/).
3. Load skill: `.cursor/skills/ecommerce-builder/SKILL.md`.

## Scope rules

- Work inside `ecommerce-platform/web/` for application code unless adding templates or docs.
- Prefer **copying from `templates/`** and adapting over inventing new patterns.
- **MongoDB + Prisma:** schema changes require `prisma generate` and documented migration notes in the SOP.
- **Auth:** preserve RBAC (`ADMIN` | `CUSTOMER`); never expose admin routes without middleware checks.
- **Payments:** never log secrets; webhook handlers must verify signatures (Stripe/RazorPay templates).
- **Forms:** React Hook Form + Zod; reuse validators from `templates/validators/`.

## Testing (Cloud Agent)

```bash
cd ecommerce-platform/web
npm run lint
npm run build
```

With MongoDB configured:

```bash
npx prisma db push
npm run dev
```

Manual UI verification: admin dashboard + storefront home, shop, product detail stubs.

## Commits

One logical change per commit (e.g. "Add coupon admin template", "Wire Prisma Order model").

## Cursor Cloud specific instructions

- Node 20+ is available; use `npm` in `ecommerce-platform/web`.
- **Full E2E (login, checkout, Prisma):** set `DATABASE_URL` to a MongoDB Atlas URI in **environment secrets** (recommended). Prisma requires a replica set; FerretDB is not sufficient for writes.
- If Atlas is configured, allow egress to `*.mongodb.net`. For in-VM `mongodb-memory-server`, allow `fastdl.mongodb.org` and `downloads.mongodb.com` (see `web/.cursor/environment.json`).
- Bootstrap: `cd ecommerce-platform/web && npm run db:local` then `npm run e2e:quick`. Headless email check: `NODE_ENV=development npx tsx scripts/test-email-dev-log.ts`.
- Without MongoDB: `npm run lint` and `npm run build` only.
- Do not run `npm audit fix --force` unless explicitly requested.
