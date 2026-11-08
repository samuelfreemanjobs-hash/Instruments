---
name: disklordz-ops-schema
description: Supabase migrations, RLS, pgvector — read-only MCP by default.
when_to_use: Profit lever — Uptime and safe schema evolution. Invoke for Disklordz Ops Schema tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Ops Schema

## When to use

Supabase migrations, RLS, pgvector — read-only MCP by default.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/ops-schema/agent.md`.
- Run automation: apply-supabase-migrations.sh

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
