---
name: disklordz-audio-rd
description: Research and experiment design for Trap/Phonk ML drums: hypotheses, ablations, dataset notes, and promotion criteria — partners with ddsp-ml-engineer (train/export) and audio-plugin-coder (JUCE ship).
when_to_use: Profit lever — De-risk ML→plugin bets before production DSP commits. Invoke for Disklordz Audio R&D tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Audio R&D

## When to use

Research and experiment design for Trap/Phonk ML drums: hypotheses, ablations, dataset notes, and promotion criteria — partners with ddsp-ml-engineer (train/export) and audio-plugin-coder (JUCE ship).

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/audio-rd/agent.md`.
- Run automation: docs/RD_EXPERIMENT_LOG.md, tools/drum-synth-blueprint/, docs/DDSP_TRAP_PHONK_AGENT_PROMPTS.md, disklordz/rag corpus

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
