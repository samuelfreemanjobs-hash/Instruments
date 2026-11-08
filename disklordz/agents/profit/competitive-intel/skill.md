---
name: disklordz-competitive-intel
description: ILLUGEN-shaped benchmarks; read-only research into docs/research/.
when_to_use: Profit lever — vs ILLUGEN positioning. Invoke for Disklordz Competitive Intel tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Competitive Intel

## When to use

ILLUGEN-shaped benchmarks; read-only research into docs/research/.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/competitive-intel/agent.md`.
- Run automation: docs/DISKLORDZ_ILLUGEN_RESEARCH.md

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
