# Walkthrough — Building the platform step by step

This guide is for **humans and AI agents**. Each session should complete **one blueprint phase** (or one SOP section) and leave the app in a runnable state.

## Session workflow (SOP-driven)

```text
1. Open docs/BLUEPRINT.md → pick next unchecked phase
2. Open docs/SOP/SOP-XX-*.md → follow steps in order
3. Copy templates from templates/ → web/src/...
4. Run: cd web && npm run lint && npm run build
5. Update BLUEPRINT checkboxes + short note in commit message
```

## Day 1 — Environment & admin shell

**Goal:** Sidebar admin with all course pages as stubs.

1. `cd ecommerce-platform/web && cp .env.example .env.local`
2. Set `DATABASE_URL` when ready; until then UI-only work is fine.
3. Copy `templates/admin/admin-layout.template.tsx` → integrate under `src/app/(admin)/admin/layout.tsx`.
4. For each route in Phase 2 of [BLUEPRINT.md](BLUEPRINT.md), add a page using the listed template.
5. Run `npm run dev` → visit `/admin` and click every nav item.

**Done when:** No 404s in admin nav; layout matches course structure (dashboard, catalog, marketing, analytics).

## Day 2 — Database & first real CRUD

**Goal:** Categories + products persisted in MongoDB.

1. Follow [SOP-03-prisma-models.md](SOP/SOP-03-prisma-models.md).
2. Implement category list/create using `templates/admin/crud-list-page.template.tsx` + server actions from `templates/server-actions/crud-actions.template.ts`.
3. `npx prisma db push` against Atlas.
4. Create one category and one product via admin.

**Done when:** Refresh shows saved data; Zod rejects empty titles.

## Day 3 — Storefront home & product page

**Goal:** Public home reads from DB; product detail works.

1. [SOP-05-storefront-foundation.md](SOP/SOP-05-storefront-foundation.md)
2. Wire home sections to `Product`, `Category`, `Offer`, `Banner` queries.
3. `/shop` and `/product/[slug]` using storefront templates.

**Done when:** Click-through from home → product → add to cart (cart can be stubbed).

## Day 4 — Auth & admin protection

**Goal:** Login/register; non-admin blocked from `/admin`.

1. [SOP-06-auth-rbac.md](SOP/SOP-06-auth-rbac.md)
2. Seed an `ADMIN` user; test customer cannot access admin.

## Day 5 — Checkout & Stripe test mode

**Goal:** Test payment completes and creates `Order`.

1. [SOP-07-cart-checkout.md](SOP/SOP-07-cart-checkout.md) + [SOP-08-payments-webhooks.md](SOP/SOP-08-payments-webhooks.md)
2. Use Stripe CLI for local webhooks if needed.

## Day 6 — Cloudinary + email

**Goal:** Product image upload; order email sent.

1. [SOP-09-cloudinary-uploads.md](SOP/SOP-09-cloudinary-uploads.md)
2. [SOP-10-order-email.md](SOP/SOP-10-order-email.md)

## Agent-specific notes

- Always cite the **SOP step number** you are executing in PR descriptions.
- Do not skip Phase 2 UI before Phase 3 if the course order matters to the user; the blueprint allows UI-first.
- Prefer extending `web/src/lib/validators/` from templates rather than inline Zod in pages.

## Evidence checklist (for Cloud Agents)

- Terminal: successful `npm run build` in `web/`
- Screenshot or recording: `/admin` dashboard + `/` storefront (when UI exists)
