---
name: disklordz-audio-plugin-coder
description: JUCE/VST3 plugin factory using Noizefield Audio Plugin Coder (APC) phases: Dream → Plan → Design → Implement → Ship; maps Instruments monorepo CMake plugins to APC workflow.
when_to_use: Profit lever — Ship native plugins without losing DSP/UI structure to ad-hoc agent edits. Invoke for Disklordz Audio Plugin Coder (APC) tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — Audio Plugin Coder (APC)

## When to use

JUCE/VST3 plugin factory using Noizefield Audio Plugin Coder (APC) phases: Dream → Plan → Design → Implement → Ship; maps Instruments monorepo CMake plugins to APC workflow.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/audio-plugin-coder/agent.md`.
- Run automation: docs/JUCE_APC_AGENT_BOOTSTRAP.md, root CMakeLists.txt, build-plugin.yml, APC submodule optional under disklordz/integrations/vendor/

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
