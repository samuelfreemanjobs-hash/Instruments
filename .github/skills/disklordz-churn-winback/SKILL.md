---
name: disklordz-churn-winback
description: Identify lapsed Pro users via Supabase; n8n/Email win-back with credit incentive.
when_to_use: Profit lever — Reactivate lapsed Pro users. Invoke for Disklordz Churn Win-back tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Churn Win-back

## When to use

Identify lapsed Pro users via Supabase; n8n/Email win-back with credit incentive.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/churn-winback/agent.md`.
- Run automation: saved_kits + billing snapshot queries

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
