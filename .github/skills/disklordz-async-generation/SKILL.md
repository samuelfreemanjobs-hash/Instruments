---
name: disklordz-async-generation
description: Inngest jobs for long generate batches, credit commit/release patterns.
when_to_use: Profit lever — Completed paid jobs without serverless timeouts. Invoke for Disklordz Async Generation tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Async Generation

## When to use

Inngest jobs for long generate batches, credit commit/release patterns.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/async-generation/agent.md`.
- Run automation: activate-integrations.sh --inngest, /api/generate/async

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
