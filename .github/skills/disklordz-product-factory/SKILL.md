---
name: disklordz-product-factory
description: Storefront packs: 01_KICKS…04_PERC, metadata, batch factory API.
when_to_use: Profit lever — Higher AOV catalog SKUs. Invoke for Disklordz Product Factory tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Product Factory

## When to use

Storefront packs: 01_KICKS…04_PERC, metadata, batch factory API.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/product-factory/agent.md`.
- Run automation: crew_product_factory.py, POST /api/factory/batch

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
