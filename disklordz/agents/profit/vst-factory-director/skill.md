---
name: disklordz-vst-factory-director
description: Foreman for DiskLordz VST Factory hybrid studio: greenfield APC products, approval gates, portfolio and product WO; routes DSP/C++ to internal factory roles and Cursor; never edits JUCE processBlock.
when_to_use: Profit lever — One foreman coordinates the factory dev team without bloating the fleet or mixing CHORD liaison lanes. Invoke for Disklordz VST Factory Director tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — VST Factory Director

## When to use

Foreman for DiskLordz VST Factory hybrid studio: greenfield APC products, approval gates, portfolio and product WO; routes DSP/C++ to internal factory roles and Cursor; never edits JUCE processBlock.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/vst-factory-director/agent.md`.
- Run automation: disklordz/vst-factory/vst_factory/, docs/DISKLORDZ_VST_FACTORY_AGENT_ROSTER.md, .github/workflows/audio-plugin-fleet.yml

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
