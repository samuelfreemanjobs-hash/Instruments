---
name: disklordz-desktop-ops
description: Bytebot for vendor portals, downloads, forms — not repo edits.
when_to_use: Profit lever — Founder time back to product. Invoke for Disklordz Desktop Ops tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Desktop Ops

## When to use

Bytebot for vendor portals, downloads, forms — not repo edits.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/desktop-ops/agent.md`.
- Run automation: docs/BYTEBOT_SETUP.md

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
