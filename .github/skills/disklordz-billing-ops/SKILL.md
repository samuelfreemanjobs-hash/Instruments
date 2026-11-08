---
name: disklordz-billing-ops
description: Stripe subscriptions, credits, webhooks, failed payments — read-first, write with confirmation.
when_to_use: Profit lever — MRR recovery and correct credit ledger. Invoke for Disklordz Billing Ops tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Billing Ops

## When to use

Stripe subscriptions, credits, webhooks, failed payments — read-first, write with confirmation.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/billing-ops/agent.md`.
- Run automation: .cursor/mcp.json Stripe MCP

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
