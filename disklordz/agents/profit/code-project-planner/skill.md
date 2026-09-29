---
name: disklordz-code-project-planner
description: Audio Programmer–style PRD author: product requirements, success metrics, phase gates, and agent handoff prompts before implementation (every product).
when_to_use: Profit lever — Fewer rework loops — agents implement against signed PRD + ARCHITECTURE. Invoke for Disklordz Code Project Planner (PRD) tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Code Project Planner (PRD)

## When to use

Audio Programmer–style PRD author: product requirements, success metrics, phase gates, and agent handoff prompts before implementation (every product).

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/code-project-planner/agent.md`.
- Run automation: docs/templates/PRD_TEMPLATE.md, docs/COMPANY_MEMORY_INDEX.md, per-product docs/PRD.md or product PRD section

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
