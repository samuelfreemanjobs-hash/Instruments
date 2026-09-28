---
name: disklordz-lane-workflow
description: LangGraph-style prompt → spec → N variations pipeline.
when_to_use: Profit lever — Consistent multi-candidate quality. Invoke for Disklordz Lane Workflow tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Lane Workflow

## When to use

LangGraph-style prompt → spec → N variations pipeline.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/lane-workflow/agent.md`.
- Run automation: langgraph_spec_flow.py, buildVariationBatch

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
