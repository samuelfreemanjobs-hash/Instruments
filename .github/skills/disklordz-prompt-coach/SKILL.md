---
name: disklordz-prompt-coach
description: RAG + GenerationSpec hints; lane vocabulary; /api/rag/suggest alignment.
when_to_use: Profit lever — Free tier quality → sign-up and Pro. Invoke for Disklordz Prompt Coach tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Prompt Coach

## When to use

RAG + GenerationSpec hints; lane vocabulary; /api/rag/suggest alignment.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/prompt-coach/agent.md`.
- Run automation: embed_and_upsert.py, hybrid RAG

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
