---
name: disklordz-web-ops
description: DiskLordz website operations: local preview, catalog kit checks, and deploy prep from DEPLOY.md. No production deploy or payment changes without confirmation.
when_to_use: Profit lever — The site and the first kit stay runnable for a real download. Invoke for Disklordz Web Ops tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Web Ops

## When to use

DiskLordz website operations: local preview, catalog kit checks, and deploy prep from DEPLOY.md. No production deploy or payment changes without confirmation.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/web-ops/agent.md`.
- Run automation: disklordz/website/DEPLOY.md, disklordz/website/docs/WEB_DESIGN_OPS.md

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
