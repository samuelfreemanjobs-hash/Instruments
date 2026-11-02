---
name: disklordz-ship-velocity
description: WO → issue → PR → CI → merge for Disklordz SaaS and integrations.
when_to_use: Profit lever — Shorter cycle time = faster revenue features. Invoke for Disklordz Ship Velocity tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Ship Velocity

## When to use

WO → issue → PR → CI → merge for Disklordz SaaS and integrations.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/ship-velocity/agent.md`.
- Run automation: Cursor Cloud, airtable-antigravity-handoff.yml

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
