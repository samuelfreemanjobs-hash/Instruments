# Cloudinary uploads (SOP-09)

## Environment

```env
NEXT_PUBLIC_CLOUDINARY_CLOUD_NAME=your-cloud
CLOUDINARY_API_KEY=...
CLOUDINARY_API_SECRET=...
```

## How it works

1. Admin-only `POST /api/uploads/cloudinary-sign` returns a signed payload (`timestamp`, `signature`, `folder`).
2. Admin forms (`ProductForm`, `CategoryForm`, `BannerForm`) upload directly to Cloudinary from the browser.
3. Returned `secure_url` values are stored on Product `images[]`, Category `image`, or Banner `image`.
4. Storefront uses `next/image` with `res.cloudinary.com` in `next.config.ts`.

## Folders

| Use | Folder |
|-----|--------|
| Products | `ecommerce/products` |
| Categories | `ecommerce/categories` |
| Website banners | `ecommerce/banners/website` |

## Unsigned alternative

This project uses **signed uploads** (no upload preset required). Create an upload preset only if you switch to unsigned client uploads.
