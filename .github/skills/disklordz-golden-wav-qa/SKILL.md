---
name: disklordz-golden-wav-qa
description: Plugin/DSP golden regression; cross-sell quality gate for Instruments.
when_to_use: Profit lever — Plugin cross-sell quality. Invoke for Disklordz Golden WAV QA tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Golden WAV QA

## When to use

Plugin/DSP golden regression; cross-sell quality gate for Instruments.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/golden-wav-qa/agent.md`.
- Run automation: python3 vst-testing-ops/run_business.py --profile dsp-only

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
