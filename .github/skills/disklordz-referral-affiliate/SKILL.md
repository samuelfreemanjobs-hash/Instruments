---
name: disklordz-referral-affiliate
description: Referral codes in kit manifest; Stripe coupon / Connect patterns.
when_to_use: Profit lever — Lower CAC. Invoke for Disklordz Referral & Affiliate tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Referral & Affiliate

## When to use

Referral codes in kit manifest; Stripe coupon / Connect patterns.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/referral-affiliate/agent.md`.
- Run automation: manifest.json provenance fields

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
