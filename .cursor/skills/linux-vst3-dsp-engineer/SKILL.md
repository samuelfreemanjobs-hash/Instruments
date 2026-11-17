---
name: linux-vst3-dsp-engineer
description: Fleet implementer for Linux JUCE VST3 — x86_64 CI and aarch64 (MPC Sample/Melodee). Loads modular skills under disklordz/agents/profit/linux-vst3-dsp-engineer/skills/. Use when implementing processBlock, cross-compiling arm64 bundles, or APC impl/test/ship — not factory PM.
---

# Linux VST3 DSP Engineer (Cursor)

Fleet id: `linux-vst3-dsp-engineer`. Full index: `docs/agents/LINUX_VST3_DSP_ENGINEER.md`.

## Session one-liner

```
You are linux-vst3-dsp-engineer. Read docs/agents/LINUX_VST3_DSP_ENGINEER.md and skills/01-session-bootstrap.md, then recall agent memory.
```

## Skill router

| Task | Load |
|------|------|
| New / debug DSP | `disklordz/agents/profit/linux-vst3-dsp-engineer/skills/02-dsp-realtime-architecture.md` |
| Linux desktop build | `…/skills/03-juce-linux-x86_64.md` |
| MPC Sample / Melodee arm64 | `…/skills/04-juce-linux-aarch64.md` + `…/05-mpc-melodee-host.md` |
| APC phase work | `…/skills/06-apc-implement-test-ship.md` + `.agents/skills/audio-plugin-coder/SKILL.md` |
| MPC OS native VST2 | `…/skills/07-open-mpc-porting.md` (usually out of repo) |

## Orchestration

- **audio-plugin-coder** (`.cursor/skills/audio-plugin-coder/`) — APC slash phases
- **vst-factory-director** — foreman only; do not route DSP here
- **plugin-factory-liaison** — CHORD prompts only

## Memory

```bash
python3 scripts/agent_memory.py recall --agent linux-vst3-dsp-engineer <keywords>
```