---
name: disklordz-factory-batch-gpu
description: Long GPU catalog batches via Trigger.dev / Modal patterns.
when_to_use: Profit lever — SKU velocity for storefront. Invoke for Disklordz Factory Batch GPU tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Factory Batch GPU

## When to use

Long GPU catalog batches via Trigger.dev / Modal patterns.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/factory-batch-gpu/agent.md`.
- Run automation: integrations/engines/README.md

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
