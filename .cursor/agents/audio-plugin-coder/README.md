# Audio Plugin Coder agent system

This folder documents the **multi-skill agent** used for VST/VSTi and cross-target
DSP in the Instruments repo.

## Orchestrator

- **Cursor rule:** `.cursor/rules/08-audio-plugin-coder.mdc`
- **Primary skill:** `.cursor/skills/audio-plugin-coder/SKILL.md`
- **Architecture spec:** `docs/dsp-architecture-specification.md`

Invoke explicitly: *“Use the Audio Plugin Coder agent to …”*

## Subagents

Subagents are **skill-bound roles**, not separate binaries. The orchestrator
loads the skill and either executes it or launches a Cursor **Task** subagent
(`generalPurpose` or `explore`) with the skill body as the task description.

See [SUBAGENTS.md](./SUBAGENTS.md) for delegation prompts and handoff contracts.

## Skill index

| Skill | Directory |
|-------|-----------|
| Orchestrator | `.cursor/skills/audio-plugin-coder/` |
| Program chain | `.cursor/skills/dsp-program-chain/` |
| Surface chain | `.cursor/skills/dsp-surface-chain/` |
| Global state | `.cursor/skills/dsp-global-state/` |
| Parameter mapping | `.cursor/skills/dsp-parameter-mapping/` |
| Complexity assessment | `.cursor/skills/dsp-complexity-assessment/` |
| JUCE scaffold | `.cursor/skills/juce-plugin-scaffold/` |
| Build verify | `.cursor/skills/plugin-build-verify/` |
| logue hardware port | `.cursor/skills/logue-hardware-port/` |

## Quick flow

```
User request
  → audio-plugin-coder (plan + tier)
  → juce-plugin-scaffold (if new plugin)
  → dsp-complexity-assessment
  → dsp-global-state (params.json)
  → dsp-program-chain + dsp-parameter-mapping (parallel OK)
  → dsp-surface-chain
  → plugin-build-verify
  → logue-hardware-port (optional)
```
