---
name: disklordz-linux-vst3-dsp-engineer
description: Architect and implement Linux VST3/VSTi plugins (JUCE APC lane): program-chain DSP, RT-safe processBlock, desktop x86_64 CI, aarch64 cross-builds for Akai MPC Sample/Melodee; pluginval and vst-testing-ops; does not own SaaS or factory PM.
when_to_use: Profit lever — Shippable arm64 and x86_64 VST3 binaries without mixing PM/foreman lanes — embedded and DAW targets from one repo discipline. Invoke for Disklordz Linux VST3 DSP Engineer tasks.
disable-model-invocation: false
context: fork
paths:
  - "vst/**"
  - "plugins/**"
  - "shared/dsp/**"
  - "tools/build-vst*.sh"
  - "cmake/toolchains/**"
  - "docs/MPC_SAMPLE_VST3.md"
  - "docs/CLAUDE_VST_ENGINEER.md"
  - "docs/agents/LINUX_VST3_DSP_ENGINEER.md"
  - "vst-testing-ops/**"
---

# Skill — Linux VST3 DSP Engineer

## When to use

- Implement or debug **JUCE VST3** on **Linux x86_64** (CI / DAW)
- Cross-build **aarch64** for **Melodee / MPC Sample**
- APC **implement → test → ship** for `vst/<Name>/`
- Architecture choices: program vs surface chain, voice limits, smoothing
- **Not** for: SaaS, factory portfolio, CHORD liaison prompts only

## Modular skills (read the matching file)

| # | File | Use when |
|---|------|----------|
| 01 | [skills/01-session-bootstrap.md](skills/01-session-bootstrap.md) | Session start |
| 02 | [skills/02-dsp-realtime-architecture.md](skills/02-dsp-realtime-architecture.md) | DSP design / RT bugs |
| 03 | [skills/03-juce-linux-x86_64.md](skills/03-juce-linux-x86_64.md) | Desktop Linux build / CI |
| 04 | [skills/04-juce-linux-aarch64.md](skills/04-juce-linux-aarch64.md) | Melodee / MPC Sample bundle |
| 05 | [skills/05-mpc-melodee-host.md](skills/05-mpc-melodee-host.md) | Host rules, SD install |
| 06 | [skills/06-apc-implement-test-ship.md](skills/06-apc-implement-test-ship.md) | Lifecycle commands |
| 07 | [skills/07-open-mpc-porting.md](skills/07-open-mpc-porting.md) | MPC OS VST2 / Open MPC |

**Index:** [docs/agents/LINUX_VST3_DSP_ENGINEER.md](../../../../docs/agents/LINUX_VST3_DSP_ENGINEER.md)

## Prerequisites

- Read `AGENTS.md`, `docs/CLAUDE_VST_ENGINEER.md`, `disklordz/agents/profit/linux-vst3-dsp-engineer/agent.md`
- `python3 scripts/agent_memory.py recall --agent linux-vst3-dsp-engineer <task>`
- JUCE: `git submodule update --init vst/JUCE`

## Success criteria

- [ ] Build evidence for each declared target (x86_64 and/or aarch64)
- [ ] `verify-mpc-sample-vst3.sh` or `file(1)` for arm bundles
- [ ] `run_business.py` when plugin C++ / DSP changed
- [ ] APC gates respected; no skipped design artifacts
- [ ] No secrets in git; `agent_memory.py check`

## Primary verify commands

```bash
./tools/build-vst.sh <Name>
./tools/build-vst-aarch64-linux.sh <Name>
./scripts/vst/verify-mpc-sample-vst3.sh path/to/Plugin.vst3
python3 vst-testing-ops/run_business.py --profile ci
python3 scripts/agent_memory.py check
```

## Anti-patterns

- Skipping APC Dream/Plan/Design before `vst/<Name>/`
- `processBlock` work by vst-factory-director or plugin-factory-liaison (wrong lane)
- Desktop x86_64 plugin on Melodee without cross-build
- Mixing Open MPC VST2 wrapper and JUCE in one product silently
- Unbounded voice count on embedded targets