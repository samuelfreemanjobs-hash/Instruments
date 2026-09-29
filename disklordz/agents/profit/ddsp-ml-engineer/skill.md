---
name: disklordz-ddsp-ml-engineer
description: Differentiable DSP and PyTorch training loops: feature-loss cloning of drums/808s, ONNX export hooks, batch synthesis for sound-factory and Serum Forge perceptual stages.
when_to_use: Profit lever — Elite timbre match at scale without hand-tuning every preset. Invoke for Disklordz DDSP / ML Engineer (PyTorch) tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — DDSP / ML Engineer (PyTorch)

## When to use

Differentiable DSP and PyTorch training loops: feature-loss cloning of drums/808s, ONNX export hooks, batch synthesis for sound-factory and Serum Forge perceptual stages.

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/ddsp-ml-engineer/agent.md`.
- Run automation: tools/drum-synth-blueprint/, tools/serum-forge/serum_forge/perceptual.py, optional torch in CI smoke only

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
