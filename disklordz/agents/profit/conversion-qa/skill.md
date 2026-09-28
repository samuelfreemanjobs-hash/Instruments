---
name: disklordz-conversion-qa
description: Protect generate → preview → checkout funnel; run Playwright MCP and verify-go-live.
when_to_use: Profit lever — Fewer broken flows → more Pro upgrades. Invoke for Disklordz Conversion QA tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Conversion QA

## When to use

Protect generate → preview → checkout funnel; run Playwright MCP and verify-go-live.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/conversion-qa/agent.md`.
- Run automation: verify-go-live.sh, scheduled CI

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
