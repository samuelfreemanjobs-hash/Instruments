---
name: apc-plan
description: APC Plan phase — architecture, program/surface chains, complexity tier, UI framework.
---

# Plan phase (`/apc-plan [Name]`)

## Prerequisites

- `plugins/<Name>/.ideas/creative-brief.md`
- `plugins/<Name>/.ideas/parameter-spec.md`

## Read

- `docs/dsp-architecture-specification.md`
- Skill: `.cursor/skills/dsp-complexity-assessment/SKILL.md`

## Write

1. `.ideas/architecture.md` with:
   - Core components
   - **Program chain** (ordered)
   - **Surface chain** (APVTS IDs)
   - **Global** context (sample rate, presets)
2. `.ideas/apc-plan.md` with complexity table → **L0–L3**

## Framework decision

Set `status.json`:

- `ui_framework`: `juce-native` (default) or `webview` (HTML UI — future)
- `framework_selection.decision` + rationale
- `validation.architecture_defined` = true
- `validation.ui_framework_selected` = true
- `complexity_tier`, `complexity_score`

## Stop

`**/apc-design <Name>**`
