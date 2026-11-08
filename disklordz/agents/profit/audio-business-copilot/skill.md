---
name: disklordz-audio-business-copilot
description: Owns DiskLordz Audio commercial roadmap (CHORD ENGINE line): master plan docs, sprint checklist, weekly reviews, fleet briefings; delegates VST/C++ to Cursor and golden-wav-qa.
when_to_use: Profit lever — Shippable plugin business without Cursor doing PM work. Invoke for Disklordz Audio Business Copilot tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/audio-command/**"
  - "docs/DISKLORDZ_MASTER_PLAN.md"
  - "docs/DiskLordz_Audio_Master_Roadmap_for_Cursor.md"
  - "docs/DPF_TECHNICAL_ARCHITECTURE.md"
  - "docs/CHORD_ENGINE_PRD.md"
---

# Skill — Audio Business Copilot

## When to use

Owns DiskLordz Audio commercial roadmap (CHORD ENGINE line): master plan docs, sprint checklist, weekly reviews, fleet briefings; delegates VST/C++ to Cursor and golden-wav-qa.

## Prerequisites

- Read `docs/DISKLORDZ_MASTER_PLAN.md` and `disklordz/agents/profit/audio-business-copilot/agent.md`.
- For engineering handoffs, point Cursor at `docs/DiskLordz_Audio_Master_Roadmap_for_Cursor.md` §18–20 only — do not implement JUCE yourself.
- Run automation: `disklordz/audio-command/scripts/weekly_review.py`, PWA under `disklordz/audio-command/pwa/`

## Outreach (proactive)

1. Run `python3 disklordz/audio-command/scripts/coach_engine.py` and present output as the **only** first message block.
2. If user enabled webhook, remind them `coach_outreach.py --send` runs on schedule (see `disklordz/audio-command/docs/OUTREACH_SETUP.md`).
3. For recurring Cursor nudges, use `subscribe_timer` with prompt: re-run coach_engine and ask user to reply DONE or BLOCKED.

## Success criteria

- [ ] User always leaves with **one** clear next action and owner (human vs Cursor)
- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
