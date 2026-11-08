---
name: disklordz-airtable-wo-triage
description: Route Airtable WOs to GitHub issues and Antigravity inbox JSON.
when_to_use: Profit lever — PM throughput. Invoke for Disklordz Airtable WO Triage tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Airtable WO Triage

## When to use

Route Airtable WOs to GitHub issues and Antigravity inbox JSON.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/airtable-wo-triage/agent.md`.
- Run automation: airtable-antigravity-handoff.yml, disklordz/antigravity/inbox/

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
