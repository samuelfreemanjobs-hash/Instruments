# SOP-11 — Launch checklist

## Security

- [ ] All secrets in hosting env (Vercel/etc.), not in git
- [ ] Webhook endpoints HTTPS only in production
- [ ] Rate limit auth routes
- [ ] CORS minimal on API routes

## Performance

- [ ] Image sizes via Cloudinary transforms
- [ ] Index MongoDB fields used in filters (slug, categoryId)

## SEO

- [ ] `metadata` on product and shop pages
- [ ] sitemap route (optional template)

## Definition of Done

- [ ] Production `npm run build` green
- [ ] Smoke test: browse → cart → pay → email

## Verification

Deploy preview URL + run checklist above.
