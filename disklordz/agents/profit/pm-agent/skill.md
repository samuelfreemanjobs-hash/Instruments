---
name: disklordz-pm-agent
description: Repo-wide fleet governance: DISKLORDZ_AGENTS.md, .github/agents/fleet.json, PM ADD; CI proves agents are registered and executable.
when_to_use: Profit lever — No hidden agents — fleet is discoverable and running. Invoke for Disklordz PM Agent (Fleet ADD) tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — PM Agent (Fleet ADD)

## When to use

Repo-wide fleet governance: DISKLORDZ_AGENTS.md, .github/agents/fleet.json, PM ADD; CI proves agents are registered and executable.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/pm-agent/agent.md`.
- Run automation: agent-fleet-governance.yml, agent-fleet-execute.yml, sync-disklordz-agent-fleet.sh

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
