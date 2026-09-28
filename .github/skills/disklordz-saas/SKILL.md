---
name: disklordz-saas
description: Edit Disklordz Drum SaaS (Next.js, Supabase, Stripe) safely. Use when changing generate, credits, RAG, or factory routes.
---

# Disklordz SaaS skill

## Before editing

1. Read `disklordz/website/ARCHITECTURE.md` and `docs/DISKLORDZ_SAAS_V0.md`.
2. For generation UX beyond v0, read `docs/DISKLORDZ_ILLUGEN_RESEARCH.md`.
3. Open `disklordz/integrations/manifest.json` for open-source wiring status.

## Commands

```bash
cd disklordz/website && npm ci && npm run build
curl -s localhost:3000/api/integrations/status | jq .runtime
```

## Rules

- Never commit secrets; document env names in `disklordz/website/.env.example` only.
- Validate `GenerationSpec` via existing parsers; rate-limit public POSTs.
- Prefer parametric engine default; remote engines only via `DISKLORDZ_*_ENGINE_URL`.
