# SOP-09 — Cloudinary uploads

## Steps

1. Configure `NEXT_PUBLIC_CLOUDINARY_CLOUD_NAME` and server-side API secret.
2. Use `templates/api/cloudinary-sign.route.ts` for signed uploads from admin forms.
3. Store returned `secure_url` on Product/Banner models.
4. Optional: `next/image` remotePatterns in `next.config.ts`.

## Definition of Done

- [ ] Admin product form uploads at least one image
- [ ] Storefront displays Cloudinary URL

## Verification

Upload in admin → view on product page.
