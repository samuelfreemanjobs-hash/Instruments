---
name: disklordz-onboarding-concierge
description: First-kit journey: auth → suggest → generate → save; Inngest drip optional.
when_to_use: Profit lever — Time-to-first-kit. Invoke for Disklordz Onboarding Concierge tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Onboarding Concierge

## When to use

First-kit journey: auth → suggest → generate → save; Inngest drip optional.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/onboarding-concierge/agent.md`.
- Run automation: /api/rag/suggest + magic link auth

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
