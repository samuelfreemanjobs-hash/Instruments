---
name: disklordz-workflow-automation
description: Generate PM_ADD roster and per-agent automation.yaml; maintain agent-fleet-health and scaffold-agent-workflows CI.
when_to_use: Profit lever — Agents run without manual babysitting. Invoke for Disklordz Workflow Automation tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Workflow Automation

## When to use

Generate PM_ADD roster and per-agent automation.yaml; maintain agent-fleet-health and scaffold-agent-workflows CI.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/workflow-automation/agent.md`.
- Run automation: scaffold_workflows.py, scaffold-agent-workflows.yml

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
