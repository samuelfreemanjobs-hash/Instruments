---
name: hermes-elite-data
description: Hermes data — Supabase migrations checklist, pgvector/RAG schema planning. Pairs with hermes-web.
---

# Hermes elite data

1. Run `python3 disklordz/hermes/scripts/hermes_tool.py data checklist`.
2. Read `disklordz/website/ARCHITECTURE.md`, `docs/RAG_AND_INTELLIGENT_AUTOMATION.md`.
3. Migrations: SQL under `disklordz/website/supabase/migrations/` — idempotent, RLS on user tables.
4. pgvector WO-SAAS-012b — document in PR; no prod credentials in repo.
5. Split work: schema/migration PRs with **hermes-web** for API wiring.
