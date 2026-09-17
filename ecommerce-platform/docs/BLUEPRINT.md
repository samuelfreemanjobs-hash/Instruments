# Master blueprint — Full-stack eCommerce (2025)

Use this as the **single source of truth** for build order. Check off items as you complete them; each phase links to an SOP.

**Legend:** `[ ]` todo · `[~]` in progress · `[x]` done

---

## Phase 0 — Introduction & prerequisites

- [ ] Confirm Node 20+, Git, MongoDB Atlas (or local), Stripe/RazorPay test keys, Cloudinary cloud
- [ ] Review [WALKTHROUGH.md](WALKTHROUGH.md) and admin/store demos (reference recordings)
- [ ] Clone / use `ecommerce-platform/web` starter

**SOP:** [SOP-00-prerequisites.md](SOP/SOP-00-prerequisites.md)

---

## Phase 1 — Cursor / AI setup

- [ ] Cursor rules: repo + `ecommerce-platform/AGENTS.md`
- [ ] Enable **ecommerce-builder** skill
- [ ] Define branch naming and “one SOP step per PR” workflow

**SOP:** [SOP-01-cursor-ai-setup.md](SOP/SOP-01-cursor-ai-setup.md)

---

## Phase 2 — Admin dashboard UI (static)

**Starter status:** Route stubs and sidebar implemented in `web/` (Phase 2 UI shell).

Match course UI modules before wiring DB:

| Module | Route stub | Template |
|--------|------------|----------|
| Dashboard | `/admin` | `templates/admin/dashboard-page.template.tsx` |
| Customers | `/admin/customers` | `templates/admin/crud-list-page.template.tsx` |
| Coupons | `/admin/coupons` | `templates/admin/crud-list-page.template.tsx` |
| Orders | `/admin/orders` | `templates/admin/crud-list-page.template.tsx` |
| Product create | `/admin/products/new` | `templates/admin/crud-form-page.template.tsx` |
| All products | `/admin/products` | `templates/admin/crud-list-page.template.tsx` |
| Categories | `/admin/categories` | `templates/admin/crud-list-page.template.tsx` |
| Sub-categories | `/admin/subcategories` | `templates/admin/crud-list-page.template.tsx` |
| Website banners | `/admin/banners/website` | `templates/admin/crud-list-page.template.tsx` |
| App banners | `/admin/banners/app` | `templates/admin/crud-list-page.template.tsx` |
| Top bar | `/admin/topbar` | `templates/admin/crud-form-page.template.tsx` |
| Home offers | `/admin/offers` | `templates/admin/crud-list-page.template.tsx` |
| Reviews | `/admin/reviews` | `templates/admin/crud-list-page.template.tsx` |
| Analytics | `/admin/analytics` | `templates/admin/analytics-page.template.tsx` |

- [x] Shared admin layout + sidebar ([`templates/admin/admin-layout.template.tsx`](../templates/admin/admin-layout.template.tsx))
- [x] All pages above render with mock data

**SOP:** [SOP-02-admin-dashboard-ui.md](SOP/SOP-02-admin-dashboard-ui.md)

---

## Phase 3 — Prisma ORM & models

- [x] `prisma/schema.prisma` — User, Product, Category, SubCategory, Order, OrderItem, Coupon, Review, Banner, TopBar, Offer
- [x] `npx prisma generate` + `db push` (use `npm run dev:db` for in-memory Mongo + push + seed)
- [x] Seed script (`prisma/seed.ts` — admin user, categories, products, coupon, top bar)

**SOP:** [SOP-03-prisma-models.md](SOP/SOP-03-prisma-models.md)

---

## Phase 4 — Admin completion (CRUD + logic)

Per entity: list → create → edit → delete → validation → toast/error states.

- [ ] Coupons (codes, expiry, min cart)
- [ ] Categories & sub-categories (nested)
- [ ] Products (images, inventory, category links)
- [ ] Edit product page
- [ ] Banners (website/app) + offers
- [ ] Top bar messaging
- [ ] Analytics queries (revenue, orders, top products)
- [ ] Reviews moderation
- [ ] Orders status workflow
- [ ] Customers list (read-only / role)

**SOP:** [SOP-04-admin-crud-completion.md](SOP/SOP-04-admin-crud-completion.md)

---

## Phase 5 — Storefront foundation

- [ ] Home (hero, categories, offers, featured products) — Part 1 & 2
- [ ] Shop listing + filters
- [ ] Product detail page
- [ ] Search modal
- [ ] Top bar integration from CMS

**SOP:** [SOP-05-storefront-foundation.md](SOP/SOP-05-storefront-foundation.md)

---

## Phase 6 — Authentication & profile

- [x] Register / login pages (RHF + Zod server actions)
- [x] Session cookies (JWT via `jose`, httpOnly)
- [x] Navbar account modal (`AccountNav`)
- [x] Profile page (name update; addresses in checkout SOP-07)
- [x] RBAC middleware for `/admin/*` + admin layout guard

**SOP:** [SOP-06-auth-rbac.md](SOP/SOP-06-auth-rbac.md)

---

## Phase 7 — Cart, favorites, checkout

- [x] Cart drawer (httpOnly cookie, guest + signed-in)
- [x] Favorites drawer (cookie-backed wishlist)
- [x] Checkout (shipping, coupon apply, `PENDING` order)
- [ ] Payment method selection (Stripe / RazorPay) — SOP-08
- [x] Order pages `/account/orders`, `/account/orders/[id]`

**SOP:** [SOP-07-cart-checkout.md](SOP/SOP-07-cart-checkout.md)

---

## Phase 8 — Payments & webhooks

- [x] Stripe Checkout sessions (`startStripeCheckoutAction`)
- [x] RazorPay order + client checkout + verify API
- [x] Webhook signature verification (`/api/webhooks/stripe`, `/api/webhooks/razorpay`)
- [x] Idempotent fulfill (`lib/payments/fulfill.ts`) + dev simulate when no Stripe keys

**SOP:** [SOP-08-payments-webhooks.md](SOP/SOP-08-payments-webhooks.md)

---

## Phase 9 — Media (Cloudinary)

- [ ] Signed upload from admin product/banner forms
- [ ] Image transformations for thumbnails

**SOP:** [SOP-09-cloudinary-uploads.md](SOP/SOP-09-cloudinary-uploads.md)

---

## Phase 10 — Email & operations

- [ ] Order confirmation email (customer)
- [ ] Admin notifications (optional)

**SOP:** [SOP-10-order-email.md](SOP/SOP-10-order-email.md)

---

## Phase 11 — Hardening & launch

- [ ] Zod on all server actions
- [ ] Rate limits on auth + checkout
- [ ] SEO metadata on product/shop pages
- [ ] Production env checklist

**SOP:** [SOP-11-launch-checklist.md](SOP/SOP-11-launch-checklist.md)

---

## Phase 12 — Future: mobile app

- [ ] Document REST/JSON contracts from existing `/api/*`
- [ ] Generate OpenAPI from route templates
- [ ] Reuse auth token strategy (separate SOP when started)

**SOP:** [SOP-12-mobile-future.md](SOP/SOP-12-mobile-future.md)

---

## Course module index (quick lookup)

| Course topic | Blueprint phase |
|--------------|-----------------|
| Cursor Setup | 1 |
| Dashboard / Customers / Coupons / Orders UI | 2 |
| Product / Category / SubCategory / Banners UI | 2 |
| TopBar / Offers / Reviews / Analytics UI | 2 |
| Prisma Setup & Models | 3 |
| *Completion* modules (coupon, categories, products, …) | 4 |
| E-commerce frontend, home, product, search | 5 |
| Auth, navbar, profile | 6 |
| Cart, checkout, orders | 7–8 |
| Email confirmation | 10 |
| TopBar on storefront | 5 |
