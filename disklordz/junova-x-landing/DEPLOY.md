# Deploy Junova-X landing (Vercel)

1. Create Vercel project rooted at `disklordz/junova-x-landing`.
2. Framework preset: **Next.js**.
3. Set env vars from `.env.example` (demo download URL when Releases exist).
4. Production domain: e.g. `junova-x.com` or `junova.disklordz.com` (DNS → Vercel).

```bash
npm ci && npm run build
```

CI: `.github/workflows/junova-landing.yml` on changes to this directory.
