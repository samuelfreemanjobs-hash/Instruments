# Templates catalog

Copy files into `web/src/` (or `web/prisma/`) and rename `.template.*` → real extensions.

## Quick apply

```bash
# Example: admin layout
cp templates/admin/admin-layout.template.tsx web/src/app/(admin)/admin/layout.tsx
# Then fix imports (@/ paths)
```

## Index

| Template | Purpose |
|----------|---------|
| **prisma/** | |
| `schema.prisma.template` | Full domain schema |
| `seed-admin.template.ts` | Seed ADMIN user |
| **admin/** | |
| `admin-layout.template.tsx` | Sidebar + shell |
| `dashboard-page.template.tsx` | KPI dashboard |
| `crud-list-page.template.tsx` | Table list |
| `crud-form-page.template.tsx` | Create form |
| `analytics-page.template.tsx` | Charts placeholder |
| **storefront/** | |
| `store-layout.template.tsx` | Navbar/footer |
| `home-page.template.tsx` | Home sections |
| `shop-page.template.tsx` | Product grid |
| `product-detail.template.tsx` | PDP |
| `search-modal.template.tsx` | Search overlay |
| `cart-drawer.template.tsx` | Cart UI |
| `auth-page.template.tsx` | Login/register |
| **validators/** | |
| `product.schema.template.ts` | Zod product |
| `coupon.schema.template.ts` | Zod coupon |
| `category.schema.template.ts` | Zod category |
| `order.schema.template.ts` | Zod checkout |
| `auth.schema.template.ts` | Login/register |
| **server-actions/** | |
| `crud-actions.template.ts` | Generic CRUD pattern |
| **api/** | |
| `products-search.route.template.ts` | GET search |
| `stripe-checkout.route.template.ts` | Checkout session |
| `stripe-webhook.route.template.ts` | Stripe webhook |
| `razorpay-order.route.template.ts` | RazorPay create |
| `cloudinary-sign.route.template.ts` | Signed upload |
| **lib/** | |
| `db.template.ts` | Prisma singleton |
| `auth-session.template.ts` | Session cookies |
| `stripe.template.ts` | Stripe client |
| `razorpay.template.ts` | RazorPay client |
| `cloudinary.template.ts` | Cloudinary config |
| `utils.template.ts` | cn() helper |
| **middleware/** | |
| `auth.middleware.template.ts` | RBAC |
| **email/** | |
| `order-confirmation.template.html` | Email body |
| **components/** | |
| `data-table.template.tsx` | Reusable table |
| `form-field.template.tsx` | RHF field wrapper |
| **env/** | |
| `.env.example.template` | All env vars |

When adding a new template, update this table and the matching SOP.
