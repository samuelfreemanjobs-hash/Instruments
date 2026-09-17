# SOP-05 — Storefront foundation

## Steps

1. Route group `(store)` with shared `layout.tsx` (navbar, footer, top bar).
2. **Home** — sections from templates: hero, categories grid, offers, featured products.
3. **Shop** — `/shop` with query filters (`category`, `sort`, `page`).
4. **Product** — `/product/[slug]` gallery, price, add-to-cart.
5. **Search modal** — client component; debounced fetch `/api/products/search`.
6. **TopBar** — load active `TopBar` document from DB.

## Templates

- `templates/storefront/home-page.template.tsx`
- `templates/storefront/shop-page.template.tsx`
- `templates/storefront/product-detail.template.tsx`
- `templates/storefront/search-modal.template.tsx`

## Definition of Done

- [ ] Home loads live catalog data
- [ ] Product page matches slug
- [ ] Search returns results

## Verification

```bash
npm run build
```

Browse `/`, `/shop`, `/product/<seed-slug>`.
