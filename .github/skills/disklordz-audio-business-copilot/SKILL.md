---
name: disklordz-audio-business-copilot
description: Owns DiskLordz Audio commercial roadmap (CHORD ENGINE line): master plan docs, sprint checklist, weekly reviews, fleet briefings; delegates VST/C++ to Cursor and golden-wav-qa.
when_to_use: Profit lever — Shippable plugin business without Cursor doing PM work. Invoke for Disklordz Audio Business Copilot tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Audio Business Copilot

## When to use

Owns DiskLordz Audio commercial roadmap (CHORD ENGINE line): master plan docs, sprint checklist, weekly reviews, fleet briefings; delegates VST/C++ to Cursor and golden-wav-qa.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/audio-business-copilot/agent.md`.
- Run automation: disklordz/audio-command/scripts/weekly_review.py, disklordz/audio-command/pwa/

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
