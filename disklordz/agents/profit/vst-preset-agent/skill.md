---
name: disklordz-vst-preset-agent
description: Monorepo JUCE instruments: wavetable design, virtual-analog voice programming, factory preset banks, DSP effect chains, and trap/plugg/phonk archetypes for JD Upgraded, WAVE-9090, Plugg VA, and SP-1200.
when_to_use: Profit lever — Plugin preset depth → differentiation vs generic wavetable competitors. Invoke for Disklordz VST Preset Agent tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — VST Preset Agent

## When to use

Monorepo JUCE instruments: wavetable design, virtual-analog voice programming, factory preset banks, DSP effect chains, and trap/plugg/phonk archetypes for JD Upgraded, WAVE-9090, Plugg VA, and SP-1200.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/vst-preset-agent/agent.md`.
- Run automation: python3 disklordz/integrations/agents/vst_preset_agent.py, scripts/vst/verify_factory_preset_counts.py, .github/workflows/vst-preset-factory.yml

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
