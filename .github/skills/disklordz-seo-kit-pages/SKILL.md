---
name: disklordz-seo-kit-pages
description: Programmatic SEO pages from factory metadata and lane docs.
when_to_use: Profit lever — Organic traffic → signups. Invoke for Disklordz SEO Kit Pages tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — SEO Kit Pages

## When to use

Programmatic SEO pages from factory metadata and lane docs.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/seo-kit-pages/agent.md`.
- Run automation: disklordz/website/src/app/ SEO routes

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
