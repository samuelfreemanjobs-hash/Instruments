# Skill 01 — Session bootstrap

## When

Start of any **implement / debug / cross-build** task for Linux VST3 or embedded Melodee targets.

## Load (in order)

1. `AGENTS.md` (monorepo index)
2. `docs/CLAUDE_VST_ENGINEER.md` (RT + build)
3. `docs/dsp-architecture-specification.md`
4. Product `ARCHITECTURE.md` + `plugins/<Name>/status.json` if APC product
5. `docs/MPC_SAMPLE_VST3.md` if target is MPC Sample / Melodee
6. `docs/agents/LINUX_VST3_DSP_ENGINEER.md` (this agent index)

## Memory

```bash
python3 scripts/agent_memory.py recall --agent linux-vst3-dsp-engineer <keywords>
```

Apply **Always load** from `disklordz/agents/memory/MEMORY.md`.

## Identify target host

| User says | Lane |
|-----------|------|
| DAW on PC Linux | x86_64 `build-vst.sh` |
| MPC Sample + Melodee | aarch64 `build-vst-aarch64-linux.sh` |
| MPC Live / Force native page | Open MPC `vst.json` path |
| Stock MPC Sample only | Read Open MPC `MPC_SAMPLE.md`; often not JUCE VST3 |

## Escalate away from this agent

- Factory portfolio / gates only → `vst-factory-director`
- CHORD Milestone prompts → `plugin-factory-liaison`
- Golden WAV refresh → `golden-wav-qa` + user approval