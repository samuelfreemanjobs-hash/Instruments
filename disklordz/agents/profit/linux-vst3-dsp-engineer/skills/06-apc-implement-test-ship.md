# Skill 06 — APC implement, test, ship

## Gates

```bash
bash tools/apc-gate.sh <Name> impl   # before vst/<Name>/ or processBlock
bash tools/apc-gate.sh <Name> ship # before package-vst.sh
```

Required artifacts under `plugins/<Name>/`: creative brief, parameter spec, architecture, design spec (`status.json` validation flags).

## Commands by phase

| Phase | Command |
|-------|---------|
| Scaffold | `./tools/new-vst.sh <Name> [--type effect]` |
| Implement | Edit `vst/<Name>/Source/` program chain first |
| Test x86_64 | `./tools/build-vst.sh <Name>` |
| Test aarch64 | `./tools/build-vst-aarch64-linux.sh <Name>` |
| QA | `python3 vst-testing-ops/run_business.py --profile ci` |
| Ship | `./tools/package-vst.sh <Name> [version]` |

Slash router: `.agents/skills/audio-plugin-coder/SKILL.md` (`/apc-impl`, `/apc-test`, `/apc-ship`).

## Definition of done

1. Builds for declared targets (x86_64 and/or aarch64)
2. Verification script or `file(1)` evidence for aarch64
3. CI-relevant tests green when DSP changed
4. No secrets; `agent_memory.py check`
5. New lesson → `agent_memory.py add --agent linux-vst3-dsp-engineer`

## Package note

`package-vst.sh` defaults to `build/` (x86_64). For Melodee-only drops, stage the `build-aarch64` bundle explicitly or extend packaging in a follow-up PR.