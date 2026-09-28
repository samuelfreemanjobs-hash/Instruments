---
name: disklordz-pricing-experiment
description: Stripe price A/B, Pro tier tests, credit pack experiments.
when_to_use: Profit lever — ARPU / Stripe A/B. Invoke for Disklordz Pricing Experiment tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Pricing Experiment

## When to use

Stripe price A/B, Pro tier tests, credit pack experiments.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/pricing-experiment/agent.md`.
- Run automation: STRIPE_PRO_PRICE_ID env variants

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
