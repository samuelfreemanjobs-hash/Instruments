---
name: disklordz-analytics-interpreter
description: Weekly MRR, gen volume, credit burn from Stripe + Supabase.
when_to_use: Profit lever — Weekly MRR/gen metrics. Invoke for Disklordz Analytics Interpreter tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Analytics Interpreter

## When to use

Weekly MRR, gen volume, credit burn from Stripe + Supabase.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/analytics-interpreter/agent.md`.
- Run automation: Slack CI notify patterns, manual CSV export

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
