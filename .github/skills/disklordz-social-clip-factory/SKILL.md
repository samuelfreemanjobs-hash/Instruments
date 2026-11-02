---
name: disklordz-social-clip-factory
description: 15s preview clips from generated WAVs for social top-of-funnel.
when_to_use: Profit lever — Top-of-funnel clips. Invoke for Disklordz Social Clip Factory tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Social Clip Factory

## When to use

15s preview clips from generated WAVs for social top-of-funnel.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/social-clip-factory/agent.md`.
- Run automation: ffmpeg batch over kit samples

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
