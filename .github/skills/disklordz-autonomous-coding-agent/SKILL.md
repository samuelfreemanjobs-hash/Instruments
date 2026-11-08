---
name: disklordz-autonomous-coding-agent
description: Always-on DiskLordz coder: one bounded slice per tick from coding-queue — Python, docs, automation, Antigravity dispatch; escalates JUCE/DPF C++ to plugin-factory-liaison; never Stripe/prod schema.
when_to_use: Profit lever — DiskLordz ships while Samuel runs the business — 24/7 queue drain without human babysitting. Invoke for Disklordz Autonomous Coding Agent (24/7) tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Autonomous Coding Agent (24/7)

## When to use

Always-on DiskLordz coder: one bounded slice per tick from coding-queue — Python, docs, automation, Antigravity dispatch; escalates JUCE/DPF C++ to plugin-factory-liaison; never Stripe/prod schema.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/autonomous-coding-agent/agent.md`.
- Run automation: scripts/autonomous-coder/tick.py, .github/workflows/autonomous-coding-agent.yml, sync-all-antigravity.sh

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
