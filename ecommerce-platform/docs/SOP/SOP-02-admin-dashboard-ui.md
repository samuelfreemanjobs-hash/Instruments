# SOP-02 — Admin dashboard UI (static)

## Steps

1. **Layout** — Apply `templates/admin/admin-layout.template.tsx` to `web/src/app/(admin)/admin/layout.tsx`.
2. **Navigation** — Match sidebar items to [BLUEPRINT Phase 2](../BLUEPRINT.md#phase-2--admin-dashboard-ui-static).
3. **Pages** — For each route, copy list or form template; pass **mock arrays** from `web/src/lib/mock/admin-mock-data.ts`.
4. **Styling** — Use Tailwind consistent with templates (slate sidebar, white content card).
5. **No database yet** — OK to defer Prisma until Phase 3.

## Route checklist

Create `page.tsx` under:

- `admin/page.tsx` (dashboard)
- `admin/customers`, `coupons`, `orders`
- `admin/products`, `admin/products/new`
- `admin/categories`, `admin/subcategories`
- `admin/banners/website`, `admin/banners/app`
- `admin/topbar`, `admin/offers`, `admin/reviews`, `admin/analytics`

## Definition of Done

- [ ] All sidebar links resolve (no 404)
- [ ] Dashboard shows KPI cards (mock numbers)
- [ ] Tables render on list pages

## Verification

```bash
cd web && npm run dev
# Manual: click every admin nav item
```
