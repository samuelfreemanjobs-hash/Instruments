# SOP-04 — Admin CRUD completion

## Pattern (repeat per entity)

1. Zod schema in `web/src/lib/validators/<entity>.ts` from `templates/validators/`.
2. Server actions from `templates/server-actions/crud-actions.template.ts`.
3. List page: fetch in Server Component, table from template.
4. Form page: React Hook Form + `zodResolver`.
5. Edit route: `[id]/page.tsx` reuses form with default values.
6. Delete: confirm dialog + server action.

## Entity order (recommended)

1. Categories → SubCategories (depends on category)
2. Products (depends on categories)
3. Coupons
4. Banners & Offers & TopBar
5. Orders (read/update status)
6. Reviews (approve/delete)
7. Customers (read-only)
8. Analytics (aggregate orders)

## Definition of Done

- [ ] Each admin module persists to MongoDB
- [ ] Validation errors show inline
- [ ] Admin-only enforced (after SOP-06)

## Verification

Create → list → edit → delete for **Product** and **Coupon** manually.
