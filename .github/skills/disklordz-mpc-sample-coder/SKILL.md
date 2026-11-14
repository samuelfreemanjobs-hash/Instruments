---
name: disklordz-mpc-sample-coder
description: Akai MPC Sample (AC50) engineering: .xpj/.xpm workflows, MPCTK and mpc-sample-sdk lanes, Melodee/sample-base and virtual-lab testing; aligns Trap Lord exports with MPCVObject; never ships firmware or AGPL melodee forks into proprietary VSTs.
when_to_use: Profit lever — Faster AC50 kits and mods — reuse Instruments renders on hardware without wrong-format exports. Invoke for Disklordz MPC Sample Coder tasks.
disable-model-invocation: false
context: fork
paths:
  - "docs/MPC_SAMPLE*.md"
  - "disklordz/trap-lord-synth/**"
  - "disklordz/agents/profit/mpc-sample-coder/**"
  - "disklordz/integrations/agents/mpc_sample_coder.py"
  - "disklordz/**"
  - ".github/workflows/**"
---

# Skill — MPC Sample Coder

## When to use

Akai MPC Sample (AC50) engineering: .xpj/.xpm workflows, MPCTK and mpc-sample-sdk lanes, Melodee/sample-base and virtual-lab testing; aligns Trap Lord exports with MPCVObject; never ships firmware or AGPL melodee forks into proprietary VSTs.

## Prerequisites

- Read `docs/MPC_SAMPLE_ECOSYSTEM.md`, `docs/MPC_SAMPLE_CODER.md`, `PLAYBOOK.md`, and `agent.md` (skip SaaS `website/ARCHITECTURE.md` unless the task touches kits on the site).
- Run automation: `python3 disklordz/integrations/agents/mpc_sample_coder.py --self-test`

## Success criteria

- [ ] Target device/format stated (Sample `.xpj` vs Live/Melodee `MPCVObject` `.xpm`)
- [ ] `mpc_sample_coder.py --self-test` passes when repo files changed
- [ ] Hardware claims tagged [executed] / [assumed]
- [ ] No firmware binaries or secrets in git

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out