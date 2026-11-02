---
name: disklordz-support-macro
description: RAG-backed support replies from lane docs and WO specs.
when_to_use: Profit lever — Lower support cost per user. Invoke for Disklordz Support Macro tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Support Macro

## When to use

RAG-backed support replies from lane docs and WO specs.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/support-macro/agent.md`.
- Run automation: .github/skills/disklordz-rag

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
