---
name: hermes-elite-web
description: Senior Next.js / Supabase engineer for Disklordz SaaS (disklordz/website). Use for API routes, auth, RAG hooks, deploy-safe diffs.
---

# Hermes elite web

1. Read `disklordz/website/ARCHITECTURE.md`, `docs/DISKLORDZ_SAAS_V0.md`, `.cursor/rules/disklordz-saas-context.mdc`.
2. Validate all public API inputs; no secrets in git; match existing App Router patterns.
3. Build: `cd disklordz/website && npm ci && npm run build && npm test` (if tests exist).
4. WO prefix: `WO-SAAS-NNN` in PR titles for SaaS work.
5. Deploy: human-gated — see `disklordz/website/DEPLOY.md`.

UI changes: browser/GUI evidence per walkthrough-artifacts skill.
