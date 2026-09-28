---
name: disklordz-hardware-preset-designer
description: SynthForge workstation: batch preset generation, prompt-to-patch, WAV cloning, SQLite lineage, safety-clamped SysEx/binary export for supported hardware synths.
when_to_use: Profit lever — Hardware preset velocity — cross-sell Instruments + producer kits. Invoke for Disklordz Hardware Preset Designer tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Hardware Preset Designer

## When to use

SynthForge workstation: batch preset generation, prompt-to-patch, WAV cloning, SQLite lineage, safety-clamped SysEx/binary export for supported hardware synths.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/hardware-preset-designer/agent.md`.
- Run automation: python3 disklordz/integrations/agents/hardware_preset_designer.py, tools/synth-forge pytest, synth-forge.yml CI

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
