# Junova-X landing (WO-GTM-001)

## Purpose

Marketing site for **Junova-X** plugin — separate from Drum SaaS (`disklordz/website/`). Static Next.js App Router; deploy to Vercel.

## Build & run

```bash
cd disklordz/junova-x-landing
npm ci
npm run build
npm run dev
```

## Env (`.env.example`)

- `NEXT_PUBLIC_DEMO_DOWNLOAD_URL` — link to Linux demo zip or GitHub Releases asset
- `NEXT_PUBLIC_DISKLORDZ_URL` — cross-link to main Disklordz site

## Data flow

Static page → CTA links only (no API in v0.1). Stripe checkout = Tier D WO on `disklordz/website` or dedicated API route later.

## Related

- [Junova-X/gtm/LANDING_COPY.md](../../Junova-X/gtm/LANDING_COPY.md)
- [Junova-X/docs/FINISH_LINE.md](../../Junova-X/docs/FINISH_LINE.md)
- [DEPLOY.md](DEPLOY.md)
