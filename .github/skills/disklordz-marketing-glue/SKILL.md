---
name: disklordz-marketing-glue
description: n8n workflows: leads, email, Slack, kit drops.
when_to_use: Profit lever — Retention and top-of-funnel. Invoke for Disklordz Marketing Glue tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Marketing Glue

## When to use

n8n workflows: leads, email, Slack, kit drops.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/marketing-glue/agent.md`.
- Run automation: docker-compose.optional.yml --profile n8n

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
