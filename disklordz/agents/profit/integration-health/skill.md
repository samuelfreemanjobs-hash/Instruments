---
name: disklordz-integration-health
description: Monitor /api/integrations/status and verify-integrations.sh.
when_to_use: Profit lever — Prevent revenue leaks from misconfig. Invoke for Disklordz Integration Health tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Integration Health

## When to use

Monitor /api/integrations/status and verify-integrations.sh.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/integration-health/agent.md`.
- Run automation: verify-integrations.sh, activate-integrations.yml

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
