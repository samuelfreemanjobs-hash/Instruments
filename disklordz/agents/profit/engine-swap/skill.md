---
name: disklordz-engine-swap
description: Remote AudioCraft / Stable Audio workers vs parametric fallback.
when_to_use: Profit lever — Pro retention via premium sound. Invoke for Disklordz Engine Swap tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Engine Swap

## When to use

Remote AudioCraft / Stable Audio workers vs parametric fallback.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/engine-swap/agent.md`.
- Run automation: DISKLORDZ_ENGINE=remote, engine docker profile

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
