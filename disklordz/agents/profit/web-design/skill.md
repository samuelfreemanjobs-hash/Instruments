---
name: disklordz-web-design
description: DiskLordz storefront design: Stitch mockup prompts, brand layout, and page specs from the crest, floppy mark, and Memphis Vault cover. Mockups only — planned modules stay labeled planned.
when_to_use: Profit lever — A storefront producers trust, without fake reviews or unfinished features sold as live. Invoke for Disklordz Web Design tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Web Design

## When to use

DiskLordz storefront design: Stitch mockup prompts, brand layout, and page specs from the crest, floppy mark, and Memphis Vault cover. Mockups only — planned modules stay labeled planned.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/web-design/agent.md`.
- Run automation: disklordz/website/docs/WEB_DESIGN_OPS.md

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
