---
name: disklordz-v-voyager-ui-designer
description: VOYAGER-V JUCE editor: implement UI_CONCEPT_VOYAGER-V.png mockup in SynthEditor; visual parity with Systems Engineering Audio panel art.
when_to_use: Profit lever — Plugin SKU polish — mockup-to-ship UI for DL-PLUGIN-V-VOYAGER. Invoke for Disklordz V Voyager UI Designer tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — V Voyager UI Designer

## When to use

VOYAGER-V JUCE editor: implement UI_CONCEPT_VOYAGER-V.png mockup in SynthEditor; visual parity with Systems Engineering Audio panel art.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/v-voyager-ui-designer/agent.md`.
- Run automation: plugin-factory/plugins/moog-voyager/agents/UI_AGENT.md, MoogVoyager_Standalone visual QA

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
