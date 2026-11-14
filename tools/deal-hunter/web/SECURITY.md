# Shipped surface (Cursor Apps publish)

- **Bundled catalog:** `src/data/hosted-catalog.json` — demo retailer URLs only; no user PII. Regenerate with `npm run export:hosted-catalog` then `npm run slim:hosted-catalog` before commit.
- **Secrets:** Supabase/admin/VAPID/n8n keys are **environment only** (Cursor Publish Secrets), not in source.
- **Auth:** Email/password via Supabase; callbacks use `safeRedirectPath` (same-site paths only).
- **Admin:** `/api/admin` and dispatch-steals blocked in middleware; admin UI gated by `BFM_ADMIN_EMAILS`.