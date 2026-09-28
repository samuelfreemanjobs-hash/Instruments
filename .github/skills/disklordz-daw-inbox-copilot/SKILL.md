---
name: disklordz-daw-inbox-copilot
description: disklordz/daw-inbox watcher UX, notifications, folder hygiene.
when_to_use: Profit lever — Producer stickiness. Invoke for Disklordz DAW Inbox Copilot tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — DAW Inbox Copilot

## When to use

disklordz/daw-inbox watcher UX, notifications, folder hygiene.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/daw-inbox-copilot/agent.md`.
- Run automation: disklordz/daw-inbox/, WO-SAAS-016

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
