---
name: disklordz-license-compliance
description: Engine licensing gates (AudioCraft NC, Stable Audio); ship checklists.
when_to_use: Profit lever — Engine licensing gates. Invoke for Disklordz License & Compliance tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — License & Compliance

## When to use

Engine licensing gates (AudioCraft NC, Stable Audio); ship checklists.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/license-compliance/agent.md`.
- Run automation: engine-swap agent + integrations/engines/

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
