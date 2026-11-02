---
name: disklordz-spec-validator
description: Validate GenerationSpec JSON before generate; pydantic-ai shaped.
when_to_use: Profit lever — Fewer bad gens and refunds. Invoke for Disklordz Spec Validator tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Spec Validator

## When to use

Validate GenerationSpec JSON before generate; pydantic-ai shaped.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/spec-validator/agent.md`.
- Run automation: pydantic_spec_agent.py, parseGenerationSpec

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
