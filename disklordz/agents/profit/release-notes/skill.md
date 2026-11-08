---
name: disklordz-release-notes
description: Customer-facing changelog from merged PRs and WO titles.
when_to_use: Profit lever — Trust + upgrades. Invoke for Disklordz Release Notes tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Release Notes

## When to use

Customer-facing changelog from merged PRs and WO titles.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/release-notes/agent.md`.
- Run automation: PR labels + docs/CHANGELOG or website news

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
