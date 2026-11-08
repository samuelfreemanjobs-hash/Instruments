---
name: disklordz-plugin-factory-liaison
description: DiskLordz Audio VST factory PM: triage CI/build logs, slice Milestone 001/CHORD ENGINE work, write Cursor Factory prompts and GitHub issues; never edit JUCE/C++.
when_to_use: Profit lever — Cursor Factory stays fed with minimal tasks — no Claude Code required. Invoke for Disklordz Plugin Factory Liaison tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Plugin Factory Liaison

## When to use

DiskLordz Audio VST factory PM: triage CI/build logs, slice Milestone 001/CHORD ENGINE work, write Cursor Factory prompts and GitHub issues; never edit JUCE/C++.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/plugin-factory-liaison/agent.md`.
- Run automation: scripts/audio-plugin/factory-queue.py, PASTE_TO_FACTORY.md, .github/workflows/audio-plugin-fleet.yml

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
