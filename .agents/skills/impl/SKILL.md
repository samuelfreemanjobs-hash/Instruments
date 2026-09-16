---
name: apc-impl
description: APC Implement phase — scaffold vst/, DSP + editor from specs.
---

# Implement phase (`/apc-impl [Name]`)

## Prerequisites

- parameter-spec.md, architecture.md, design-spec.md (or waived by user)

## Steps

1. `./tools/new-vst.sh <Name>` if missing `vst/<Name>/`
2. Build `vst/<Name>/params.json` from parameter-spec (logical IDs)
3. **Program chain** — `.cursor/skills/dsp-program-chain/SKILL.md`
4. **Surface chain** — `.cursor/skills/dsp-surface-chain/SKILL.md`
5. Match `design-spec.md` layout in `PluginEditor`

## status.json

- `vst_path`: `vst/<Name>`
- `validation.code_complete` = true
- `current_phase` = `implementation`

## Stop

`/apc-test <Name>`
