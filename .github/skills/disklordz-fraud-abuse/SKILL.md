---
name: disklordz-fraud-abuse
description: Rate limits, IP heuristics, guest gen abuse; protect margin.
when_to_use: Profit lever — Protect free-tier margin. Invoke for Disklordz Fraud & Abuse tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Fraud & Abuse

## When to use

Rate limits, IP heuristics, guest gen abuse; protect margin.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/fraud-abuse/agent.md`.
- Run automation: src/lib/rate-limit.ts, SAAS_DAILY_GEN_LIMIT

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
